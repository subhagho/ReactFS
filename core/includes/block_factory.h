//
// Created by Subhabrata Ghosh on 31/10/16.
//

#ifndef REACTFS_BLOCK_FACTORY_H
#define REACTFS_BLOCK_FACTORY_H

#include "fs_block.h"
#include "fs_typed_block.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                template<class T>
                class block_factory {
                public:
                    fs_block *open_block(uint64_t block_id, string filename, __block_record_type type) {
                        fs_block *block = nullptr;
                        switch (type) {
                            case __block_record_type::RAW:
                                block = new fs_block();
                                block->open(block_id, filename);
                            case __block_record_type::TYPED:
                                block = new fs_typed_block<T>();
                                block->open(block_id, filename);
                        }
                        return block;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_BLOCK_FACTORY_H
