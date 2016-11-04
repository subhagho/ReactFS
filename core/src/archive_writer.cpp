//
// Created by Subhabrata Ghosh on 04/11/16.
//

#include "core/includes/archive_writer.h"

#define COMPRESS_BLOCK_SIZE 128 * 1024

__compressed_data *
com::wookler::reactfs::core::archive_writer::write_archive_data(void *source, uint64_t in_size,
                                                                __compression_type type) {
    CHECK_NOT_NULL(source);
    PRECONDITION(in_size > 0);

    __compressed_data *ret = nullptr;
    switch (type) {
        case __compression_type::GZIP:
            ret = write_gzip_data(source, in_size);
            break;
        case __compression_type::ZLIB:
            ret = write_zlib_data(source, in_size);
            break;
        case __compression_type::LZO:
            ret = write_lzo_data(source, in_size);
            break;
        case __compression_type::SNAPPY:
            ret = write_snappy_data(source, in_size);
            break;
        default:
            throw FS_ARCHIVAL_ERROR("Unsupported compression type. [type=%d]", type);
    }
    POSTCONDITION(NOT_NULL(ret));
    POSTCONDITION(ret->size > 0);

    return ret;
}

__compressed_data *
com::wookler::reactfs::core::archive_writer::write_zlib_data(void *source, uint64_t in_size) {
    CHECK_NOT_NULL(source);
    PRECONDITION(in_size > 0);

    std::vector<uint8_t> buffer;


    const size_t BUFSIZE = (in_size > COMPRESS_BLOCK_SIZE ? COMPRESS_BLOCK_SIZE : (in_size + 32));
    uint8_t temp_buffer[BUFSIZE];

    z_stream _z_stream;
    _z_stream.zalloc = 0;
    _z_stream.zfree = 0;
    _z_stream.next_in = static_cast<Bytef *>(source);
    _z_stream.avail_in = in_size;
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

    POSTCONDITION(deflate_res == Z_STREAM_END);
    buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - _z_stream.avail_out);
    deflateEnd(&_z_stream);

    POSTCONDITION(buffer.size() > 0);
    uint8_t *data = buffer.data();
    __compressed_data *c_data = (__compressed_data *) malloc(sizeof(__compressed_data));
    POSTCONDITION(NOT_NULL(c_data));
    c_data->size = buffer.size();
    c_data->data = (uint8_t *) malloc(c_data->size * sizeof(uint8_t));
    POSTCONDITION(NOT_NULL(c_data->data));

    memcpy(c_data->data, data, c_data->size);

    return c_data;
}

__compressed_data *
com::wookler::reactfs::core::archive_writer::write_gzip_data(void *source, uint64_t in_size) {
    return nullptr;
}

__compressed_data *
com::wookler::reactfs::core::archive_writer::write_lzo_data(void *source, uint64_t in_size) {
    return nullptr;
}

__compressed_data *
com::wookler::reactfs::core::archive_writer::write_snappy_data(void *source, uint64_t in_size) {
    return nullptr;
}