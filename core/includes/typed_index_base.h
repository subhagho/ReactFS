//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_TYPED_INDEX_BASE_H
#define REACTFS_TYPED_INDEX_BASE_H

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

REACTFS_NS_CORE
                typedef struct __index_key__ {
                    BYTE *bytes = nullptr;
                    uint16_t size = 0;
                } __index_key;

                typedef struct __index_key_set__ {
                    __index_key *keys = nullptr;
                    uint8_t count = 0;
                } __index_key_set;

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
                public:
                    /*!
                     * Base virtual destructor.
                     */
                    virtual ~typed_index_base() {
                        this->close();
                    }

                    /*!
                     * Create a new file backed data block index.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @param estimated_records - Estimated number of records the data file is expected to have.
                     * @param start_index - Starting record index for this block.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - Filename of the index file created.
                     */
                    virtual string
                    create_index(uint64_t block_id, string block_uuid, string filename, uint32_t estimated_records,
                                 uint64_t start_index,
                                 bool overwrite) = 0;

                    /*!
                     * Open a new instance of the specified data block index.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void open_index(uint64_t block_id, string block_uuid, string filename) = 0;

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
                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_INDEX_BASE_H
