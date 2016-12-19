//
// Created by Subhabrata Ghosh on 15/11/16.
//

#include "core/includes/compression_zlib.h"

int com::wookler::reactfs::core::compression_zlib::read_archive_data(void *source, uint64_t in_size,
                                                                     temp_buffer *out_buffer) {
    CHECK_NOT_NULL(source);
    CHECK_NOT_NULL(out_buffer);
    PRECONDITION(in_size > 0);

    z_stream _z_stream = {0};
    _z_stream.total_in = _z_stream.avail_in = in_size;
    _z_stream.total_out = _z_stream.avail_out = out_buffer->get_size();
    _z_stream.next_in = static_cast<Bytef *>(source);
    _z_stream.next_out = static_cast<Bytef *>(out_buffer->get_ptr());

    _z_stream.zalloc = Z_NULL;
    _z_stream.zfree = Z_NULL;
    _z_stream.opaque = Z_NULL;

    int err = -1;
    int ret = -1;

    err = inflateInit2(&_z_stream,
                       (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
    if (err == Z_OK) {
        err = inflate(&_z_stream, Z_FINISH);
        if (err == Z_STREAM_END) {
            ret = _z_stream.total_out;
        } else {
            inflateEnd(&_z_stream);
            return err;
        }
    } else {
        inflateEnd(&_z_stream);
        return err;
    }

    inflateEnd(&_z_stream);

    return ret;
}

void com::wookler::reactfs::core::compression_zlib::write_archive_data(void *source, uint64_t in_size,
                                                                       temp_buffer *out_buffer) {
    CHECK_NOT_NULL(source);
    CHECK_NOT_NULL(out_buffer);

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

    out_buffer->copy(data, buffer.size());
    out_buffer->set_used_size(buffer.size());
}