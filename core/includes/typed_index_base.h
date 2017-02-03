//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_TYPED_INDEX_BASE_H
#define REACTFS_TYPED_INDEX_BASE_H

#include <openssl/md5.h>

#include "common/includes/common.h"
#include "common/includes/__state__.h"
#include "common/includes/log_utils.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/mapped_data.h"
#include "common/includes/buffer_utils.h"
#include "types/includes/schema_def.h"

#include "core.h"
#include "common_structs.h"
#include "fs_error_base.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;
using namespace com::wookler::reactfs::core::types;

REACTFS_NS_CORE

                typedef struct __typed_index_record__ {
                    uint8_t state = BLOCK_RECORD_STATE_FREE;
                    uint32_t hash_value = 0;
                    uint64_t data_offset = 0;
                    uint64_t data_size = 0;
                    void *key_data_ptr = nullptr;
                } __typed_index_record;

                typedef struct __typed_index_read__ {
                    __typed_index_record *record = nullptr;
                    uint64_t bucket_offset = 0;
                    uint8_t record_index = 0;
                } __typed_index_read;

                struct __typed_index_header_v0__ {
                    char block_uid[SIZE_UUID + 1];
                    __version_header version;
                    uint64_t block_id;
                    __index_type_enum type;
                    uint64_t create_time = 0;
                    uint64_t update_time = 0;
                    uint64_t start_index = 0;
                    uint64_t last_index = 0;
                    __write_state write_state = __write_state::WRITABLE;
                    uint64_t total_size = 0;
                    uint64_t used_size = 0;
                    uint64_t write_offset = 0;
                };


                class __typed_index_bucket_v0__ {
                private:
                    uint64_t *bucket_offset = nullptr;
                    uint8_t *bucket_size = nullptr;
                    bool *has_next_ptr = nullptr;
                    uint64_t *next_ptr_offset = nullptr;
                    uint8_t *record_count = nullptr;
                    __typed_index_record *key_set = nullptr;

                    record_index *index_def = nullptr;

                public:
                    __typed_index_bucket_v0__(record_index *index_def) {
                        CHECK_NOT_NULL(index_def);
                        this->index_def = index_def;
                    }

                    ~__typed_index_bucket_v0__() {
                    }

                    bool has_overflow() {
                        CHECK_NOT_NULL(this->has_next_ptr);
                        return *this->has_next_ptr;
                    }

                    record_index *get_record_index_def() {
                        CHECK_NOT_NULL(index_def);
                        return index_def;
                    }

                    uint64_t get_overflow_offset() {
                        CHECK_NOT_NULL(this->has_next_ptr);
                        if (*this->has_next_ptr) {
                            CHECK_NOT_NULL(this->next_ptr_offset);
                            return *this->next_ptr_offset;
                        }
                        throw BASE_ERROR("Record does not have overflow.");
                    }

                    void set_overflow_offset(uint64_t offset) {
                        PRECONDITION(!has_next_ptr);
                        PRECONDITION(*record_count == *bucket_size);

                        *has_next_ptr = true;
                        *next_ptr_offset = offset;
                    }

                    uint64_t get_bucket_offset() {
                        CHECK_NOT_NULL(this->bucket_offset);
                        return *this->bucket_offset;
                    }

                    bool can_write_index() {
                        if (!has_overflow()) {
                            return (*record_count < *bucket_size);
                        }
                        return false;
                    }

                    void read(void *buffer, uint64_t offset, uint16_t key_size) {
                        CHECK_NOT_NULL(buffer);

                        __pos pos;
                        pos.offset = offset;
                        pos.size = 0;

                        // Read bucket offset.
                        pos.size += buffer_utils::read<uint64_t>(buffer, &pos.offset, &this->bucket_offset);
                        CHECK_NOT_NULL(this->bucket_offset);

                        // Read bucket size
                        pos.size += buffer_utils::read<uint8_t>(buffer, &pos.offset, &this->bucket_size);
                        CHECK_NOT_NULL(this->bucket_size);
                        POSTCONDITION(*this->bucket_size > 0);

                        // Read has overflow
                        pos.size += buffer_utils::read<bool>(buffer, &pos.offset, &this->has_next_ptr);
                        CHECK_NOT_NULL(this->has_next_ptr);

                        // Read overflow offset
                        pos.size += buffer_utils::read(buffer, &pos.offset, &this->next_ptr_offset);
                        CHECK_NOT_NULL(this->next_ptr_offset);

                        // Read record count
                        pos.size += buffer_utils::read<uint8_t>(buffer, &pos.offset, &this->record_count);
                        CHECK_NOT_NULL(this->record_count);

                        // For record ket set
                        for (uint8_t ii = 0; ii < *this->bucket_size; ii++) {
                            void *ptr = buffer_utils::increment_data_ptr(buffer, pos.offset);
                            __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);
                            if (IS_NULL(this->key_set)) {
                                this->key_set = rec;
                            }
                            pos.offset += sizeof(__typed_index_record) + key_size;
                            pos.size += sizeof(__typed_index_record) + key_size;
                        }
                    }

                    __typed_index_record *write(uint32_t hash, __index_key_set *key, uint64_t offset, uint64_t size) {
                        PRECONDITION(can_write_index());
                        CHECK_NOT_NULL(key);
                        CHECK_NOT_NULL(key->key_data);
                        CHECK_NOT_NULL(this->key_set);

                        void *ptr = this->key_set;
                        uint64_t incr = (sizeof(__typed_index_record) + *key->key_size) * (*record_count);
                        ptr = buffer_utils::increment_data_ptr(ptr, incr);
                        __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);
                        POSTCONDITION(rec->state == BLOCK_RECORD_STATE_FREE);

                        rec->state = BLOCK_RECORD_STATE_USED;
                        rec->hash_value = hash;
                        rec->data_offset = offset;
                        rec->data_size = size;
                        memcpy(rec->key_data_ptr, key->key_data, *key->key_size);

                        *this->record_count += 1;

                        return rec;
                    }

                    __typed_index_record *
                    write_commit(uint8_t bucket_cell, uint16_t key_size) {
                        void *ptr = this->key_set;
                        uint64_t incr = (sizeof(__typed_index_record) + key_size) * (bucket_cell);
                        ptr = buffer_utils::increment_data_ptr(ptr, incr);
                        __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);

                        POSTCONDITION(rec->state == BLOCK_RECORD_STATE_USED);
                        rec->state = BLOCK_RECORD_STATE_READABLE;

                        return rec;
                    }

                    void write_rollback(uint8_t bucket_cell, uint16_t key_size) {
                        void *ptr = this->key_set;
                        uint64_t incr = (sizeof(__typed_index_record) + key_size) * (bucket_cell);
                        ptr = buffer_utils::increment_data_ptr(ptr, incr);
                        __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);

                        POSTCONDITION(rec->state == BLOCK_RECORD_STATE_USED);
                        rec->state = BLOCK_RECORD_STATE_FREE;
                        *this->record_count -= 1;
                    }

                    void write_extend_rollback() {
                        *this->has_next_ptr = false;
                        *this->next_ptr_offset = 0;
                    }

                    __typed_index_record *
                    update(uint32_t hash, uint8_t bucket_cell, uint64_t offset, uint64_t size, uint16_t key_size) {
                        PRECONDITION(can_write_index());
                        CHECK_NOT_NULL(this->key_set);

                        void *ptr = this->key_set;
                        uint64_t incr = (sizeof(__typed_index_record) + key_size) * (bucket_cell);
                        ptr = buffer_utils::increment_data_ptr(ptr, incr);
                        __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);
                        POSTCONDITION(rec->hash_value == hash);

                        rec->state = BLOCK_RECORD_STATE_READABLE;
                        rec->hash_value = hash;
                        rec->data_offset = offset;
                        rec->data_size = size;

                        return rec;
                    }

                    void remove(uint32_t hash, uint8_t bucket_cell, uint16_t key_size) {
                        uint64_t incr = (sizeof(__typed_index_record) + key_size) * bucket_cell;
                        void *ptr = this->key_set;
                        ptr = buffer_utils::increment_data_ptr(ptr, incr);
                        __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);

                        PRECONDITION(
                                rec->state == BLOCK_RECORD_STATE_USED || rec->state == BLOCK_RECORD_STATE_READABLE);
                        PRECONDITION(rec->hash_value == hash);

                        rec->state = BLOCK_RECORD_STATE_DELETED;
                    }

                    __typed_index_record *
                    find(uint32_t hash, __index_key_set *key, uint8_t *offset,
                         uint8_t state = BLOCK_RECORD_STATE_READABLE) {
                        CHECK_NOT_NULL(key);
                        CHECK_NOT_NULL(key->key_data);
                        CHECK_NOT_NULL(this->key_set);
                        if (*record_count > 0) {
                            void *ptr = nullptr;
                            for (uint8_t ii = 0; ii < *record_count; ii++) {
                                uint32_t off = (sizeof(__typed_index_record) + *key->key_size) * ii;
                                ptr = buffer_utils::increment_data_ptr(this->key_set, off);
                                __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);
                                if (rec->state == BLOCK_RECORD_STATE_FREE) {
                                    throw FS_BASE_ERROR("Hash index corrupted : Record state is free.");
                                }
                                if (rec->state == state || state == BLOCK_RECORD_STATE_FREE) {
                                    if (rec->hash_value == hash) {
                                        if (memcmp(rec->key_data_ptr, key->key_data, *key->key_size) == 0) {
                                            *offset = ii;
                                            return rec;
                                        }
                                    }
                                }
                            }
                        }
                        return nullptr;
                    }

                    static uint32_t
                    init_index_record(void *buffer, uint64_t offset, uint64_t base_offset, uint8_t key_set_size,
                                      uint16_t key_size) {
                        CHECK_NOT_NULL(buffer);
                        __pos pos;
                        pos.offset = offset;
                        pos.size = 0;

                        // Write the base offset of this record.
                        pos.size += buffer_utils::write<uint64_t>(buffer, &pos.offset, base_offset);
                        // Write record size
                        pos.size += buffer_utils::write<uint8_t>(buffer, &pos.offset, key_set_size);

                        bool bvalue = false;
                        // Write the has overflow value
                        pos.size += buffer_utils::write<bool>(buffer, &pos.offset, bvalue);
                        uint64_t lvalue = 0;
                        // Write the overflow offset value
                        pos.size += buffer_utils::write<uint64_t>(buffer, &pos.offset, lvalue);
                        uint8_t svalue = 0;
                        // Write the used records count.
                        pos.size += buffer_utils::write<uint8_t>(buffer, &pos.offset, svalue);

                        // For record ket set
                        for (uint8_t ii = 0; ii < key_set_size; ii++) {
                            void *ptr = buffer_utils::increment_data_ptr(buffer, pos.offset);
                            __typed_index_record *rec = static_cast<__typed_index_record *>(ptr);

                            rec->state = BLOCK_RECORD_STATE_FREE;
                            rec->hash_value = 0;
                            rec->data_offset = 0;
                            rec->data_size = 0;
                            ptr = buffer_utils::increment_data_ptr(ptr, (pos.offset + sizeof(__typed_index_record)));
                            rec->key_data_ptr = ptr;
                            memset(rec->key_data_ptr, 0, key_size);
                            pos.offset += sizeof(__typed_index_record) + (key_size * sizeof(char));
                            pos.size += sizeof(__typed_index_record) + (key_size * sizeof(char));
                        }
                        return pos.size;
                    }

                    static uint16_t
                    get_index_record_size(uint8_t key_set_size, uint16_t key_size) {
                        uint16_t size = 0;
                        // Bucket index
                        size += sizeof(uint64_t);
                        // Bucket offset
                        size += sizeof(uint64_t);
                        // Has overflow flag
                        size += sizeof(bool);
                        // Overflow offset
                        size += sizeof(uint64_t);
                        // Used records count
                        size += sizeof(uint8_t);

                        uint16_t k_size = sizeof(__typed_index_record) + (key_size * sizeof(char));

                        size += (k_size * key_set_size);

                        return size;
                    }
                };

                typedef __typed_index_bucket_v0__ __typed_index_bucket;
                typedef __typed_index_header_v0__ __typed_index_header;

                class typed_index_base {
                protected:
                    __version_header version;

                    //! State of this instance of the block object.
                    __state__ state;

                    //! Backing file path for the memory mapped file.
                    string filename;

                    /// Memory-mapped file handle.
                    file_mapped_data *mm_data = nullptr;

                    /// Base pointer for the mapped data.
                    void *base_ptr = nullptr;

                    void *write_ptr = nullptr;

                    __typed_index_header *header = nullptr;

                    record_index *index_def = nullptr;

                    uint64_t header_offset = 0;

                    const __complex_type *datatype = nullptr;

                    __native_type **types = nullptr;

                    /*!
                     * Get the base address pointing to where the block data starts.
                     *
                     * @return - Base data address
                     */
                    void *get_data_ptr() {
                        return buffer_utils::increment_data_ptr(base_ptr, header_offset);
                    }

                    Path *get_index_file(string block_file, uint64_t block_id, string index_name) {
                        CHECK_NOT_EMPTY(block_file);
                        CHECK_NOT_EMPTY(index_name);

                        Path bp = Path(block_file);
                        const string pp = bp.get_parent_dir();

                        Path *p = new Path(pp);
                        CHECK_ALLOC(p, TYPE_NAME(Path));
                        PRECONDITION(p->exists());

                        string filename = common_utils::format("%lu_%s.index", block_id, index_name.c_str());
                        p->append(filename);

                        return p;
                    }

                    string get_metric_name(const string name) {
                        if (NOT_NULL(header)) {
                            return common_utils::format("%s.%lu", name.c_str(), header->block_id);
                        }
                        return common_consts::EMPTY_STRING;
                    }

                public:
                    /*!
                     * Base virtual destructor.
                     */
                    virtual ~typed_index_base() = default;

                    const char *get_index_name() {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(index_def);
                        return index_def->get_name();
                    }

                    record_index *get_index_def() {
                        HECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(index_def);
                        return index_def;
                    }

                    /*!
                     * Create a new file backed data block index.
                     *
                     * @param name - Name of this index definition.
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @param estimated_records - Estimated number of records the data file is expected to have.
                     * @param start_index - Starting record index for this block.
                     * @param index_def - Index definition.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - Filename of the index file created.
                     */
                    virtual string
                    create_index(const string &name, uint64_t block_id, string block_uuid, string filename,
                                 uint32_t estimated_records, uint64_t start_index, record_index *index_def,
                                 bool overwrite) = 0;

                    /*!
                     * Open a new instance of the specified data block index.
                     *
                     * @param name - Name of this index definition.
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @param for_update - Block is write closed, but opened for update/delete.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void
                    open_index(const string &name, uint64_t block_id, string block_uuid, string filename,
                               bool for_update) = 0;

                    /*!
                    * Commit the current transcation.
                    *
                    * @param transaction_id - Transaction ID obtained via a start_transaction call.
                    */
                    virtual void commit(string txid) = 0;

                    /*!
                     * Rollback the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    virtual void rollback(string txid) = 0;

                    /*!
                     * Force a transaction rollback.
                     *
                     * Should be used only in exception cases, such as transaction timeout.
                     */
                    virtual void force_rollback() = 0;

                    /*!
                     * Delete the specified index record.
                     *
                     * @param index - Index key of Record to delete.
                     * @param transaction_id - Current transaction ID.
                     * @return - Is deleted?
                     */
                    virtual bool delete_index(__index_key_set *index, string transaction_id) = 0;

                    /*!
                     * Check if this index block can be written to.
                     *
                     * @return - Index can be written to?
                     */
                    virtual const bool has_write_space() const = 0;

                    /*!
                     * Create a new index record for the specified index and offset.
                     *
                     * @param index - Record sequence index.
                     * @param offset - Read offset in the data file.
                     * @param size - Size of the data record in bytes.
                     * @param transaction_id - Current transaction ID.
                     * @return - Created index pointer.
                     */
                    virtual const __typed_index_record *
                    write_index(__index_key_set *index, uint64_t offset, uint64_t size, string transaction_id) = 0;

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param index - Data record index.
                     * @param rec_state - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    virtual const __typed_index_record *
                    read_index(__index_key_set *index, uint8_t rec_state = BLOCK_RECORD_STATE_READABLE) = 0;

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param indexs - Vector of Data record index.
                     * @param rec_state - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    virtual vector<const __typed_index_record *> *read_index(const vector<__index_key_set *> indexs,
                                                                             uint8_t rec_state = BLOCK_RECORD_STATE_READABLE) = 0;

                    /*!
                     * Close this instance of the block index.
                     */
                    virtual void close() = 0;

                    virtual void sync(bool recreate = false) = 0;
                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_INDEX_BASE_H
