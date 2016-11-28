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
                    static string
                    create_new_block(uint64_t block_id, string filename, __block_type type, uint32_t block_size,
                                     uint32_t est_record_size, uint64_t start_index) {
                        base_block *block = new base_block();
                        CHECK_ALLOC(block, TYPE_NAME(base_block));
                        string uuid = block->create(block_id, filename, type, block_size, start_index, est_record_size,
                                                    false);
                        POSTCONDITION(!IS_EMPTY(uuid));
                        CHECK_AND_FREE(block);

                        return uuid;
                    }

                    static void delete_block(uint64_t block_id, string filename) {
                        base_block *block = new base_block();
                        CHECK_ALLOC(block, TYPE_NAME(base_block));
                        block->open(block_id, filename);
                        CHECK_AND_FREE(block);

                        Path p(filename);
                        if (p.exists()) {
                            p.remove();
                        }
                    }

                    static string get_block_dir(mount_client *m_client) {
                        CHECK_NOT_NULL(m_client);


                        string mount = m_client->get_next_mount();
                        POSTCONDITION(!IS_EMPTY(mount));

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
