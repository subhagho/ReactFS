//
// Created by Subhabrata Ghosh on 28/10/16.
//


/*
 * Copyright [yyyy] [name of copyright owner]
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

#ifndef REACTFS_BLOCK_ARCHIVER_H
#define REACTFS_BLOCK_ARCHIVER_H

#include <zlib.h>

#include "common/includes/file_utils.h"
#include "fs_block.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class block_archiver {
                private:
                    string archive_gzip(fs_block *block);

                    string archive_zlib(fs_block *block);

                    string archive_lzo(fs_block *block);

                    string archive_snappy(fs_block *block);

                    string archive_file(string source_path, Path *dest_dir);

                public:
                    string
                    archive(fs_block *block, __compression_type compression, __archival archive, string archive_dir);

                };
            }
        }
    }
}


#endif //REACTFS_BLOCK_ARCHIVER_H
