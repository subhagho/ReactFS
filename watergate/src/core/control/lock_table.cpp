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

#include "common/includes/lock_record_def.h"
#include "watergate/includes/lock_table.h"

void com::watergate::core::lock_table::create(string name, resource_def *resource, bool server) {
    try {
        PRECONDITION(!IS_EMPTY(name));

        this->name = common_utils::get_normalized_name(name);

        string l_name(LOCK_TABLE_LOCK_PREFIX);
        l_name.append(this->name);

        LOG_DEBUG("Creating shared memory. [name=%s]", this->name.c_str());

        lock = new exclusive_lock(&l_name);
        lock->create();

        mode_t mode = O_CREAT | O_RDWR;
        if (!server) {
            mode = O_RDWR;
        }

        shm_fd = shm_open(this->name.c_str(), mode, 0760);
        if (shm_fd < 0) {
            lock_table_error te = LOCK_TABLE_ERROR(
                    "Error creating shared memory handle. [name=%s][error=%s]",
                    this->name.c_str(), strerror(errno));
            LOG_ERROR(te.what());
            throw te;
        }
        /* configure the size of the shared memory segment */
        int size = sizeof(_lock_table);
        ftruncate(shm_fd, size);
        LOG_DEBUG("Sizing shared memory segment. [size=%d]", size);

        /* now map the shared memory segment in the address space of the process */
        mem_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (mem_ptr == MAP_FAILED) {
            lock_table_error te = LOCK_TABLE_ERROR(
                    "Error mapping shared memory segment. [name=%s][error=%s]",
                    this->name.c_str(), strerror(errno));
            LOG_ERROR(te.what());
            throw te;
        }

        state.set_state(Available);
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

void com::watergate::core::lock_table::remove_record(int index) {

    PRECONDITION(index >= 0 && index < DEFAULT_MAX_RECORDS);

    _lock_table *ptr = (_lock_table *) mem_ptr;
    _lock_record *rec = &ptr->records[index];

    if (!lock->wait_lock()) {
        lock_table_error te = LOCK_TABLE_ERROR("Error getting lock to update table. [name=%s][error=%s]", name.c_str(),
                                               strerror(errno));
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }
    if (!rec->used) {
        lock_table_error te = LOCK_TABLE_ERROR("Table index corrupted. Returned record is being used. [index=%d]",
                                               index);
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }
    for (int ii = 0; ii < DEFAULT_MAX_RECORDS; ii++) {
        if (ptr->free_indexes[ii] < 0) {
            ptr->free_indexes[ii] = index;
            break;
        }
    }

    RESET_RECORD(rec);

    if (!lock->release_lock()) {
        lock_table_error te = LOCK_TABLE_ERROR("Error releasing lock for update table. [name=%s][error=%s]",
                                               name.c_str(),
                                               strerror(errno));
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }
}

_lock_record *com::watergate::core::lock_table::create_new_record(string app_name, string app_id, pid_t pid) {

    _lock_table *ptr = (_lock_table *) mem_ptr;
    if (!lock->wait_lock()) {
        lock_table_error te = LOCK_TABLE_ERROR("Error getting lock to update table. [name=%s][error=%s]", name.c_str(),
                                               strerror(errno));
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }
    int index = -1;
    for (int ii = 0; ii < DEFAULT_MAX_RECORDS; ii++) {
        if (ptr->free_indexes[ii] >= 0) {
            index = ptr->free_indexes[ii];
            ptr->free_indexes[ii] = FREE_INDEX_USED;
            break;
        }
    }
    if (!lock->release_lock()) {
        lock_table_error te = LOCK_TABLE_ERROR("Error releasing lock for update table. [name=%s][error=%s]",
                                               name.c_str(),
                                               strerror(errno));
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }

    if (index < 0) {
        return nullptr;
    }

    _lock_record *rec = &ptr->records[index];
    if (rec->used) {
        lock_table_error te = LOCK_TABLE_ERROR("Table index corrupted. Returned record is being used. [index=%d]",
                                               index);
        LOG_CRITICAL(te.what());
        state.set_error(&te);

        throw te;
    }
    RESET_RECORD(rec);

    rec->used = true;
    sprintf(rec->app.app_id, "%s", app_id.c_str());
    sprintf(rec->app.app_name, "%s", app_name.c_str());
    rec->app.proc_id = pid;
    rec->app.register_time = time_utils::now();

    return rec;
}