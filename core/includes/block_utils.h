//
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_BLOCK_UTILS_H
#define REACTFS_BLOCK_UTILS_H

#include "common_structs.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class block_utils {
                public:
                    static string get_compression_ext(__compression_type type) {
                        switch (type) {
                            case __compression_type::GZIP:
                                return ".gz";
                            case __compression_type::LZO:
                                return ".lz";
                            case __compression_type::SNAPPY:
                                return ".snappy";
                            case __compression_type::ZLIB:
                                return ".z";
                            default:
                                break;
                        }
                        return EMPTY_STRING;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_BLOCK_UTILS_H
