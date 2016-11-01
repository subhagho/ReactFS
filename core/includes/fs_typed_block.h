//
// Created by Subhabrata Ghosh on 27/10/16.
//


/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef REACTFS_FS_TYPED_BLOCK_H
#define REACTFS_FS_TYPED_BLOCK_H

#include "fs_block.h"
#include "common/includes/log_utils.h"
#include "common_structs.h"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                template<class T>
                class row {
                private:
                    uint64_t rowid = 0;
                    const T *data = nullptr;
                public:
                    row(uint64_t rowid, const T *data) {
                        this->rowid = rowid;
                        this->data = data;
                    }

                    ~row() {
                        data = nullptr;
                    }

                    const uint64_t get_rowid() const {
                        return this->rowid;
                    }

                    const T *get_data() const {
                        return this->data;
                    }
                };

                template<class T>
                class fs_typed_block : public fs_block {
                protected:
                    uint64_t *next_rowid = nullptr;

                    virtual void *get_data_ptr(void *base_ptr) override {
                        char *cptr = static_cast<char *>(base_ptr);
                        void *rptr = cptr + sizeof(__block_header);

                        next_rowid = static_cast<uint64_t *>(rptr);
                        LOG_DEBUG("Current used rowid = %lu", *next_rowid);

                        return cptr + sizeof(__block_header) + sizeof(uint64_t);
                    }

                public:
                    virtual string
                    create(uint64_t block_id, string filename, __block_type block_type, uint64_t block_size,
                           bool overwrite) override {
                        return create(block_id, filename, block_type, block_size, false, __compression_type::NONE, 0,
                                      overwrite);
                    }

                    virtual string
                    create(uint64_t block_id, string filename, __block_type block_type, uint64_t block_size,
                           bool compressed, __compression_type compression_type, uint64_t raw_size,
                           bool overwrite) override;

                    virtual string open(uint64_t block_id, string filename) override;

                    row<T> *read_t(uint64_t offset = 0);

                    row<T> **write_t(T **source, uint64_t count = 1);

                    virtual string
                    create_compressed_block(uint64_t size,
                                            __compression_type compression_type) override;
                };


            }
        }
    }
}

template<class T>
string
com::wookler::reactfs::core::fs_typed_block<T>::create(uint64_t block_id, string filename, __block_type block_type,
                                                       uint64_t block_size,
                                                       bool compressed, __compression_type compression_type,
                                                       uint64_t raw_size,
                                                       bool overwrite) {
    PRECONDITION(block_size > 0);
    try {
        if (!compressed) {
            uint64_t r_size = sizeof(row<T>);
            uint64_t q = block_size / r_size;
            PRECONDITION(q > 0);
            uint64_t rem = block_size % r_size;
            if (rem > 0) {
                q += 1;
            }
            block_size = q * r_size + sizeof(uint64_t);
        }
        string uuid = _create(block_id, filename, block_type, __block_record_type::TYPED, block_size, overwrite);
        if (compressed) {
            header->writable = false;
            header->compression.compressed = true;
            header->compression.uncompressed_size = raw_size;
            header->compression.type = compression_type;
        }

        void *base_ptr = stream->data();

        char *cptr = static_cast<char *>(base_ptr);
        void *rptr = cptr + sizeof(__block_header);

        next_rowid = static_cast<uint64_t *>(rptr);
        *next_rowid = 0;

        delete (block_lock);

        close();

        header = nullptr;

        return uuid;
    } catch (const exception &e) {
        throw FS_BASE_ERROR_E(e);
    } catch (...) {
        throw FS_BASE_ERROR("Unkown error occurred while creating block.");
    }
}

template<class T>
row <T> **com::wookler::reactfs::core::fs_typed_block<T>::write_t(T **source, uint64_t count) {
    CHECK_NOT_NULL(stream);
    CHECK_NOT_NULL(header);
    CHECK_NOT_NULL(source);
    PRECONDITION(count > 0);
    PRECONDITION(header->writable);

    row<T> **rows = new row<T> *[count];

    if (block_lock->wait_lock()) {
        try {
            for (int ii = 0; ii < count; ii++) {
                const T *sptr = source[ii];
                CHECK_NOT_NULL(sptr);
                PRECONDITION((header->used_bytes + sizeof(row<T>)) <= header->block_size);

                uint64_t rowid = *this->next_rowid;
                LOG_DEBUG("Current used rowid = %lu", rowid);
                row<T> *r = new row<T>(rowid, sptr);

                void *ptr = fs_block::write_r(r, sizeof(row<T>));
                row<T> *tptr = static_cast<row<T> *>(ptr);

                rows[ii] = tptr;
                rowid++;
                *this->next_rowid = rowid;

            }

        } catch (const exception &e) {
            block_lock->release_lock();
            throw FS_BASE_ERROR_E(e);
        } catch (...) {
            block_lock->release_lock();
            throw FS_BASE_ERROR("Unhandled type exception.");
        }
        block_lock->release_lock();
        return rows;
    } else {
        throw FS_BASE_ERROR("Error getting write lock on file. [block id=%s]", header->block_uid);
    }
}

template<class T>
row <T> *com::wookler::reactfs::core::fs_typed_block<T>::read_t(uint64_t offset) {
    uint64_t size = sizeof(row<T>);
    offset = offset * sizeof(row<T>);

    CHECK_NOT_NULL(stream);
    CHECK_NOT_NULL(header);
    if ((offset + size) > header->block_size) {
        return nullptr;
    }

    char *ptr = static_cast<char *>(data_ptr);
    if (offset > 0)
        ptr += offset;

    if (NOT_NULL(ptr)) {
        void *vptr = ptr;
        row<T> *tptr = static_cast<row<T> *>(vptr);

        return tptr;
    }
    return nullptr;
}

template<class T>
string com::wookler::reactfs::core::fs_typed_block<T>::open(uint64_t block_id, string filename) {
    try {
        void *base_ptr = _open(block_id, filename);

        PRECONDITION(header->record_type == __block_record_type::TYPED);


        data_ptr = get_data_ptr(base_ptr);
        POSTCONDITION(NOT_NULL(data_ptr));

        char *wptr = static_cast<char *>(data_ptr);
        if (header->write_offset > 0) {
            write_ptr = wptr + header->write_offset;
        } else {
            write_ptr = wptr;
        }
        if (header->compression.compressed) {
            char *cptr = static_cast<char *>(base_ptr);
            void *rptr = cptr + sizeof(__block_header);

            next_rowid = static_cast<uint64_t *>(rptr);
            *next_rowid = 999999;

            LOG_DEBUG("Current used rowid = %lu", *next_rowid);

            read_compressed_block(base_ptr);
        }
        return header->block_uid;
    } catch (const exception &e) {
        throw FS_BASE_ERROR_E(e);
    } catch (...) {
        throw FS_BASE_ERROR("Unkown error occurred while opening block.");
    }
}

template<class T>
string com::wookler::reactfs::core::fs_typed_block<T>::create_compressed_block(uint64_t size,
                                                                               __compression_type compression_type) {
    Path p(get_filename());
    string newf = p.get_filename();
    newf.append(block_utils::get_compression_ext(compression_type));

    Path np(p.get_parent_dir());
    np.append(newf);

    fs_typed_block<T> *block = new fs_typed_block<T>();
    block->create(this->header->block_id, np.get_path(), this->header->block_type, size, true, compression_type,
                  header->used_bytes, true);

    return string(np.get_path());
}

#endif //REACTFS_FS_TYPED_BLOCK_H
