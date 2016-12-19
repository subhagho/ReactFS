/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
//
// Created by Subhabrata Ghosh on 23/09/16.
//

#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "common/includes/lock_record_def.h"
#include "watergate/includes/lock_table.h"

bool
com::wookler::watergate::core::lock_table::create(string name, resource_def *resource, uint32_t count, bool server,
                                                  bool overwrite) {
    try {
        PRECONDITION(!IS_EMPTY(name));

        this->name = common_utils::get_normalized_name(name);

        string l_name(WG_LOCK_TABLE_LOCK_PREFIX);
        l_name.append(this->name);

        LOG_DEBUG("Creating shared memory. [name=%s]", this->name.c_str());

        INIT_LOCK_P(lock, &l_name);
        if (overwrite) {
            lock->reset();
        }
        WAIT_LOCK_GUARD(lock, 0);
        try {
            const __env *env = env_utils::get_env();
            CHECK_NOT_NULL(env);

            const Path *p = env->get_work_dir();
            Path np(p->get_path());
            np.append(WG_LOCK_TABLE_DIR);
            if (!np.exists()) {
                np.create(0755);
            }
            string filename = common_utils::format("%s.%s", this->name.c_str(), WG_LOCK_TABLE_EXT);
            np.append(filename);
            if (np.exists()) {
                if (server && overwrite) {
                    np.remove();
                }
            } else {
                if (!server) {
                    throw LOCK_ERROR("Lock client cannot create mapped file. [path=%s]", np.get_path().c_str());
                }
                overwrite = true;
            }

            uint64_t h_size = sizeof(__lock_table_header);
            uint64_t b_size = com::wookler::reactfs::common::__bitset::get_byte_size(count);
            uint64_t r_size = count * sizeof(__lock_record);
            uint64_t t_size = (h_size + b_size + r_size);

            LOG_DEBUG("Creating index file with size = %lu. [file=%s]", t_size, np.get_path().c_str());
            stream = new fmstream();
            CHECK_ALLOC(stream, TYPE_NAME(fmstream));
            stream->open(np.get_path().c_str(), t_size);

            base_ptr = stream->data();
            if (server && overwrite) {
                memset(base_ptr, 0, t_size);
            }
            header_ptr = reinterpret_cast<__lock_table_header *>(base_ptr);
            if (server && overwrite) {
                header_ptr->max_records = count;
                header_ptr->used_record = 0;
            }

            void *ptr = common_utils::increment_data_ptr(base_ptr, h_size);
            uint32_t *iptr = reinterpret_cast<uint32_t *>(ptr);
            bit_index = new com::wookler::reactfs::common::__bitset(iptr, b_size);
            CHECK_ALLOC(bit_index, TYPE_NAME(__bitset));

            ptr = common_utils::increment_data_ptr(ptr, b_size);
            record_ptr = reinterpret_cast<__lock_record *>(ptr);

        } catch (const exception &ei) {
            throw LOCK_TABLE_ERROR("ERROR : %s", ei.what());
        } catch (...) {
            throw LOCK_TABLE_ERROR("Un-typed exception triggered.");
        }
        state.set_state(Available);
        return overwrite;
    } catch (const exception &e) {
        lock_table_error lte = LOCK_TABLE_ERROR("Error creating Lock Table. [name=%s][error=%s]",
                                                name.c_str(),
                                                e.what());
        state.set_error(&lte);
        LOG_CRITICAL(lte.what());
        throw lte;
    } catch (...) {
        lock_table_error lte = LOCK_TABLE_ERROR("Error creating Lock Table. [name=%s][error=%s]",
                                                name.c_str(),
                                                "Unknown error...");
        LOG_CRITICAL(lte.what());
        state.set_error(&lte);
        throw lte;
    }
}

void com::wookler::watergate::core::lock_table::remove_record(uint32_t index) {

    PRECONDITION(index >= 0 && index < header_ptr->max_records);
    PRECONDITION(bit_index->check(index));

    WAIT_LOCK_GUARD(lock, 0);

    __lock_record *rec = (record_ptr + index);
    RESET_RECORD(rec);
    header_ptr->used_record--;
    bit_index->clear(index);
}

__lock_record *com::wookler::watergate::core::lock_table::create_new_record(string app_name, string app_id, pid_t pid) {
    if (header_ptr->used_record >= header_ptr->max_records) {
        return nullptr;
    }
    WAIT_LOCK_GUARD(lock, 0);
    int index = bit_index->get_free_bit();
    if (index >= 0)
        header_ptr->used_record++;
    RELEASE_LOCK_GUARD(0);

    if (index < 0) {
        return nullptr;
    }
    __lock_record *rec = (record_ptr + index);
    CHECK_NOT_NULL(rec);
    if (rec->used) {
        lock_table_error te = LOCK_TABLE_ERROR("Table index corrupted. Returned record is being used. [index=%d]",
                                               index);
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }
    RESET_RECORD(rec);

    rec->used = true;
    rec->index = index;
    sprintf(rec->app.app_id, "%s", app_id.c_str());
    sprintf(rec->app.app_name, "%s", app_name.c_str());
    rec->app.proc_id = pid;
    rec->app.register_time = time_utils::now();

    return rec;
}