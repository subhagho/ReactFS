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
#include "archive_writer.h"
#include "core/includes/block_factory.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                template<class T>
                class block_archiver {
                private:
                    archive_writer writer;

                    string archive_file(string source_path, Path *dest_dir);

                public:
                    fs_block *
                    archive(fs_block *block, __compression_type compression, __archival archive, string archive_dir);

                };
            }
        }
    }
}

template<class T>
fs_block *com::wookler::reactfs::core::block_archiver<T>::archive(fs_block *block, __compression_type compression,
                                                                  __archival archive, string archive_dir) {
    CHECK_NOT_NULL(block);

    try {
        if (archive == __archival::DISABLED) {
            return block;
        }
        if (archive == __archival::DELETE) {
            block->remove();
            return nullptr;
        }
        string new_block_path;

        fs_block *n_block = nullptr;
        if (compression != __compression_type::NONE) {
            block->with_archival_type(__archival::COMPRESSED);
            PRECONDITION(!block->is_compressed());

            void *ptr = block->get_data_ptr();
            CHECK_NOT_NULL(ptr);

            __compressed_data *data = writer.write_archive_data(ptr, block->get_used_space(), compression);
            POSTCONDITION(NOT_NULL(data));
            POSTCONDITION(data->size > 0);
            POSTCONDITION(NOT_NULL(data->data));

            string nb = block->create_compressed_block(data->size, __compression_type::ZLIB);
            CHECK_NOT_EMPTY(nb);
            LOG_DEBUG("Compressed block. [file=%s][data size=%lu][compressed size=%lu]", block->get_filename().c_str(),
                      block->get_used_space(), data->size);

            block_factory<T> factory;
            n_block = factory.open_block(block->get_block_id(), nb, block->get_record_type());
            POSTCONDITION(NOT_NULL(n_block));
            n_block->write_c(data->data, data->size);

            CHECK_AND_FREE(data->data);
            CHECK_AND_FREE(data);

            new_block_path = n_block->get_filename();

            block->remove();

        } else {
            new_block_path = block->get_filename();
        }

        if (archive == __archival::ARCHIVE) {
            CHECK_NOT_EMPTY(archive_dir);
            Path p(archive_dir);
            if (!p.exists()) {
                p.create(0755);
            }
            if (!p.is_directory()) {
                throw FS_ARCHIVAL_ERROR("Specified destination path is not a valid directory. [path=%s]",
                                        archive_dir.c_str());
            }
            fs_block *b = (IS_NULL(n_block) ? block : n_block);
            b->flush();

            uint64_t block_id = b->get_block_id();
            __block_record_type record_type = b->get_record_type();
            string o_filename = string(b->get_filename());
            CHECK_AND_FREE(n_block);

            new_block_path = block_archiver::archive_file(new_block_path, &p);

            block_factory<T> factory;
            fs_block *a_block = factory.open_block(block_id, new_block_path, record_type);
            POSTCONDITION(NOT_NULL(a_block));

            file_utils::remove_file(o_filename, true);

            n_block = a_block;
        }

        if (NOT_NULL(n_block) && (block->get_filename() != n_block->get_filename())) {
            return n_block;
        }
        return block;
    } catch (const exception &e) {
        throw FS_ARCHIVAL_ERROR_E(e);
    } catch (...) {
        throw FS_ARCHIVAL_ERROR("Unhandled error occurred while archiving.");
    }
}

template<class T>
string com::wookler::reactfs::core::block_archiver<T>::archive_file(string source_path, Path *dest_dir) {
    CHECK_NOT_EMPTY(source_path);
    PRECONDITION(dest_dir->exists());

    Path source(source_path);
    string outf = source.get_filename();
    PRECONDITION(!IS_EMPTY(outf));

    dest_dir->append(outf);

    LOG_INFO("Copying block file. [source=%s][dest=%s]", source.get_path().c_str(), dest_dir->get_path().c_str());
    file_copy::copy(&source, dest_dir);

    string ss(dest_dir->get_path());

    return ss;
}

#endif //REACTFS_BLOCK_ARCHIVER_H
