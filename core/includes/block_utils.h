//
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_BLOCK_UTILS_H
#define REACTFS_BLOCK_UTILS_H

#include "common_structs.h"
#include "base_block.h"

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
                        string uuid = block->create(block_id, filename, type, block_size, start_index, est_record_size,
                                                    false);
                        POSTCONDITION(!IS_EMPTY(uuid));
                        CHECK_AND_FREE(block);

                        return uuid;
                    }

                    static void delete_block(uint64_t block_id, string filename) {
                        base_block *block = new base_block();
                        block->open(block_id, filename);
                        CHECK_AND_FREE(block);

                        Path p(filename);
                        if (p.exists()) {
                            p.remove();
                        }
                    }
                };
            }
        }
    }
}
#endif //REACTFS_BLOCK_UTILS_H
