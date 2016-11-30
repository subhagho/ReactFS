//
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_BLOCK_UTILS_H
#define REACTFS_BLOCK_UTILS_H


#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common_structs.h"
#include "base_block.h"
#include "clients/mount_client.h"

using namespace com::wookler::reactfs::core::client;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class block_utils {
                public:
                    /*!
                     * Create a new basic block based on the specified parameters.
                     *
                     * @param block_id - Block id of the new block.
                     * @param filename - Filename to be used for this block.
                     * @param type - Type of the block.
                     * @param block_size - Data size for this block.
                     * @param est_record_size - Estimated size for the records in this block. (to be used to estimate index size)
                     * @param start_index - Start index for the records in this block.
                     * @return - UUID of the new block created.
                     */
                    static string
                    create_new_block(uint64_t block_id, string filename, __block_usage type, uint32_t block_size,
                                     uint32_t est_record_size, uint64_t start_index) {
                        base_block *block = new base_block();
                        CHECK_ALLOC(block, TYPE_NAME(base_block));
                        string uuid = block->create(block_id, filename, type, block_size, start_index, est_record_size,
                                                    false);
                        POSTCONDITION(!IS_EMPTY(uuid));
                        CHECK_AND_FREE(block);

                        return uuid;
                    }

                    /*!
                     * Delete the specified block from the file system.
                     *
                     * @param m_client - Mount client handle.
                     * @param block_id - Block ID of the block to be deleted.
                     * @param filename - File name of the block file.
                     */
                    static void delete_block(mount_client *m_client, uint64_t block_id, string filename) {
                        base_block *block = new base_block();
                        CHECK_ALLOC(block, TYPE_NAME(base_block));
                        block->open(block_id, filename);
                        CHECK_AND_FREE(block);

                        Path p(filename);
                        if (p.exists()) {
                            uint64_t f_size = file_utils::get_file_size(p.get_path());
                            LOG_INFO("Deleting block file. [filename=%s][size=%lu]", p.get_path().c_str(), f_size);
                            m_client->release_block(p.get_path_p(), f_size);
                            p.remove();
                        }
                    }

                    /*!
                     * Get the directory for a new block being created.
                     *
                     * @param m_client - Mount client handle.
                     * @param size - Total file size estimated for this block.
                     * @return - Directory path where the block should be created.
                     */
                    static string get_block_dir(mount_client *m_client, uint64_t size) {
                        CHECK_NOT_NULL(m_client);


                        string mount = m_client->get_next_mount(size);
                        if (IS_EMPTY(mount)) {
                            throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_OUT_OF_SPACE, "Requested size = %lu",
                                                 size);
                        }

                        if (!m_client->reserve_block(&mount, size)) {
                            throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_ALLOCATING_BLOCK_SPACE, "[mount=%s][size=%lu]",
                                                 mount.c_str(), size);
                        }

                        if (!string_utils::ends_with(&mount, "/")) {
                            mount.append("/");
                        }
                        Path path(mount);
                        POSTCONDITION(path.exists());

                        string dir = time_utils::get_hour_dir();
                        POSTCONDITION(!IS_EMPTY(dir));
                        path.append(dir);
                        if (!path.exists()) {
                            path.create(DEFAULT_RESOURCE_MODE);
                        }

                        string p(file_utils::cannonical_path(path.get_path()));
                        TRACE("Returned block location : [%s]", p.c_str());

                        return p;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_BLOCK_UTILS_H
