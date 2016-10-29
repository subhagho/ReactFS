//
// Created by Subhabrata Ghosh on 28/10/16.
//

#include "core/includes/block_archiver.h"

#define COMPRESS_BLOCK_SIZE 128 * 1024

using namespace com::wookler::reactfs::core;

string com::wookler::reactfs::core::block_archiver::archive(fs_block *block, __compression_type compression,
                                                            __archival archive, string archive_dir) {
    CHECK_NOT_NULL(block);
    try {
        if (archive == __archival::DISABLED) {
            return block->get_filename();
        }
        if (archive == __archival::DELETE) {
            block->remove();
            return EMPTY_STRING;
        }
        string new_block_path;

        if (compression != __compression_type::NONE) {
            block->with_archival_type(__archival::COMPRESSED);
            switch (compression) {
                case __compression_type::GZIP:
                    new_block_path = block_archiver::archive_gzip(block);
                    break;
                case __compression_type::LZO:
                    new_block_path = block_archiver::archive_lzo(block);
                    break;
                case __compression_type::ZLIB:
                    new_block_path = block_archiver::archive_zlib(block);
                    break;
                case __compression_type::SNAPPY:
                    new_block_path = block_archiver::archive_snappy(block);
                    break;
                default:
                    throw FS_ARCHIVAL_ERROR("Unsupported compression type. [type=%d]", compression);
            }
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
        }

        block->close();

        return new_block_path;
    } catch (const exception &e) {
        throw FS_ARCHIVAL_ERROR_E(e);
    } catch (...) {
        throw FS_ARCHIVAL_ERROR("Unhandled error occurred while archiving.");
    }
}

string com::wookler::reactfs::core::block_archiver::archive_file(string source_path, Path *dest_dir) {
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

string com::wookler::reactfs::core::block_archiver::archive_zlib(fs_block *block) {
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
        assert(res == Z_OK);
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

    assert(deflate_res == Z_STREAM_END);
    buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - _z_stream.avail_out);
    deflateEnd(&_z_stream);

    uint8_t *data = buffer.data();

    string nb = block->create_compressed_block(data, buffer.size(), __compression_type::ZLIB);
    CHECK_NOT_EMPTY(nb);
    LOG_DEBUG("Compressed block. [file=%s][data size=%lu][compressed size=%lu]", block->get_filename().c_str(),
              block->get_used_space(), buffer.size());

    return nb;
}

string com::wookler::reactfs::core::block_archiver::archive_gzip(fs_block *block) {
    throw FS_ARCHIVAL_ERROR("GZip compression not implemented.");
}

string com::wookler::reactfs::core::block_archiver::archive_lzo(fs_block *block) {
    throw FS_ARCHIVAL_ERROR("LZO compression not implemented.");
}

string com::wookler::reactfs::core::block_archiver::archive_snappy(fs_block *block) {
    throw FS_ARCHIVAL_ERROR("Snappy compression not implemented.");
}