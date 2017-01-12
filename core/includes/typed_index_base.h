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
                typedef struct __index_key__ {
                    BYTE *bytes = nullptr;
                    uint16_t size = 0;
                } __index_key;

                typedef struct __index_key_set__ {
                    __index_key *keys = nullptr;
                    uint8_t count = 0;
                } __index_key_set;

                struct __typed_index_record_v0__ {
                    unsigned char digest[MD5_DIGEST_LENGTH];
                    void *data_ptr = nullptr;
                };

                struct __typed_index_bucket_v0__ {
                    uint64_t bucket_index = 0;
                    bool has_next_ptr = false;
                    uint64_t next_ptr_offset = 0;
                    uint16_t record_count = 0;
                    void *data_ptr = nullptr;
                };

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

                typedef __typed_index_record_v0__ __typed_index_record;
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

                    uint32_t compute_index_record_size(record_index *index_def) {
                        CHECK_NOT_NULL(index_def);
                        uint32_t size = 0;
                        for (uint8_t ii = 0; ii < index_def->get_column_count(); ii++) {
                            const __index_column *column = index_def->get_index(ii);
                            CHECK_NOT_NULL(column);
                            PRECONDITION(__type_enum_helper::is_native(column->type->get_datatype()));
                            if (column->type->get_datatype() == __type_def_enum::TYPE_STRING) {
                                size += UCHAR_MAX;
                            } else {
                                size += column->type->estimate_size();
                            }
                        }
                        return size;
                    }

                public:
                    /*!
                     * Base virtual destructor.
                     */
                    virtual ~typed_index_base() = default;

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
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void
                    open_index(const string &name, uint64_t block_id, string block_uuid, string filename) = 0;

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
                     * Get the available free space that this block has.
                     *
                     * @return - Space available (in bytes).
                     */
                    virtual const uint64_t get_free_space() const = 0;

                    /*!
                     * Get the space currently used by this block.
                     *
                     * @return - Space used (in bytes).
                     */
                    virtual const uint64_t get_used_space() const = 0;

                    /*!
                     * Create a new index record for the specified index and offset.
                     *
                     * @param index - Record sequence index.
                     * @param offset - Read offset in the data file.
                     * @param size - Size of the data record in bytes.
                     * @param transaction_id - Current transaction ID.
                     * @return - Created index pointer.
                     */
                    virtual const __record_index_ptr *
                    write_index(__index_key_set *index, uint64_t offset, uint64_t size, string transaction_id) = 0;

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param index - Data record index.
                     * @param all - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    virtual const __record_index_ptr *read_index(__index_key_set *index, bool all = false) = 0;

                    /*!
                     * Close this instance of the block index.
                     */
                    virtual void close() = 0;

                    virtual void sync(bool recreate = false) = 0;
                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_INDEX_BASE_H
