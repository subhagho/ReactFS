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
#include "core/includes/block_factory.h"

#define COMPRESS_BLOCK_SIZE 128 * 1024

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                template<class T>
                class block_archiver {
                private:
                    fs_block *archive_gzip(fs_block *block);

                    fs_block *archive_zlib(fs_block *block);

                    fs_block *archive_lzo(fs_block *block);

                    fs_block *archive_snappy(fs_block *block);

                    string archive_file(string source_path, Path *dest_dir);

                public:
                    fs_block*
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
            switch (compression) {
                case __compression_type::GZIP:
                    n_block = block_archiver::archive_gzip(block);
                    break;
                case __compression_type::LZO:
                    n_block = block_archiver::archive_lzo(block);
                    break;
                case __compression_type::ZLIB:
                    n_block = block_archiver::archive_zlib(block);
                    break;
                case __compression_type::SNAPPY:
                    n_block = block_archiver::archive_snappy(block);
                    break;
                default:
                    throw FS_ARCHIVAL_ERROR("Unsupported compression type. [type=%d]", compression);
            }
            POSTCONDITION(NOT_NULL(n_block));
            new_block_path = n_block->get_filename();
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
            new_block_path = block_archiver::archive_file(new_block_path, &p);
            fs_block *b = (IS_NULL(n_block) ? block : n_block);

            block_factory<T> factory;
            fs_block *a_block = factory.open_block(b->get_block_id(), new_block_path, b->get_record_type());
            POSTCONDITION(NOT_NULL(a_block));

            if (NOT_NULL(n_block)) {
                n_block->remove();
                delete (n_block);
            }

            n_block = a_block;
        }

        if (NOT_NULL(n_block) && block->get_filename() != n_block->get_filename()) {
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

template<class T>
fs_block *com::wookler::reactfs::core::block_archiver<T>::archive_zlib(fs_block *block) {

    PRECONDITION(!block->is_compressed());
    std::vector<uint8_t> buffer;

    void *ptr = block->get_data_ptr();
    CHECK_NOT_NULL(ptr);

    uint64_t data_size = block->get_used_space();
    const size_t BUFSIZE = (data_size > COMPRESS_BLOCK_SIZE ? COMPRESS_BLOCK_SIZE : (data_size + 32));
    uint8_t temp_buffer[BUFSIZE];

    z_stream _z_stream;
    _z_stream.zalloc = 0;
    _z_stream.zfree = 0;
    _z_stream.next_in = reinterpret_cast<Bytef *>(ptr);
    _z_stream.avail_in = data_size;
    _z_stream.next_out = temp_buffer;
    _z_stream.avail_out = BUFSIZE;

    deflateInit(&_z_stream, Z_BEST_COMPRESSION);

    while (_z_stream.avail_in != 0) {
        int res = deflate(&_z_stream, Z_NO_FLUSH);
        PRECONDITION(res == Z_OK);
        if (_z_stream.avail_out == 0) {
            buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
            _z_stream.next_out = temp_buffer;
            _z_stream.avail_out = BUFSIZE;
        }
    }

    int deflate_res = Z_OK;
    while (deflate_res == Z_OK) {
        if (_z_stream.avail_out == 0) {
            buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
            _z_stream.next_out = temp_buffer;
            _z_stream.avail_out = BUFSIZE;
        }
        deflate_res = deflate(&_z_stream, Z_FINISH);
    }

    PRECONDITION(deflate_res == Z_STREAM_END);
    buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - _z_stream.avail_out);
    deflateEnd(&_z_stream);

    uint8_t *data = buffer.data();

    string nb = block->create_compressed_block(buffer.size(), __compression_type::ZLIB);
    CHECK_NOT_EMPTY(nb);
    LOG_DEBUG("Compressed block. [file=%s][data size=%lu][compressed size=%lu]", block->get_filename().c_str(),
              block->get_used_space(), buffer.size());

    block_factory<T> factory;
    fs_block *new_block = factory.open_block(block->get_block_id(), nb, block->get_record_type());
    POSTCONDITION(NOT_NULL(new_block));
    new_block->write_c(data, buffer.size());

    return new_block;
}

template<class T>
fs_block *com::wookler::reactfs::core::block_archiver<T>::archive_gzip(fs_block *block) {
    throw FS_ARCHIVAL_ERROR("GZip compression not implemented.");
}

template<class T>
fs_block *com::wookler::reactfs::core::block_archiver<T>::archive_lzo(fs_block *block) {
    throw FS_ARCHIVAL_ERROR("LZO compression not implemented.");
}

template<class T>
fs_block *com::wookler::reactfs::core::block_archiver<T>::archive_snappy(fs_block *block) {
    throw FS_ARCHIVAL_ERROR("Snappy compression not implemented.");
}


#endif //REACTFS_BLOCK_ARCHIVER_H
