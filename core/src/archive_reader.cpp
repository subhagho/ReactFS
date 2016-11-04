//
// Created by Subhabrata Ghosh on 28/10/16.
//

#include "core/includes/archive_reader.h"
#include <iostream>

void com::wookler::reactfs::core::archive_reader::read_archive_data(void *source, uint64_t in_size, void *dest,
                                                                    uint64_t out_size,
                                                                    __compression_type type) {
    CHECK_NOT_NULL(source);
    PRECONDITION(in_size > 0);
    CHECK_NOT_NULL(dest);
    PRECONDITION(out_size > 0);

    int ret = 0;
    switch (type) {
        case __compression_type::GZIP:
        case __compression_type::ZLIB:
            ret = read_zlib_data(source, in_size, dest, out_size);
            break;
        case __compression_type::LZO:
            ret = read_lzo_data(source, in_size, dest, out_size);
            break;
        case __compression_type::SNAPPY:
            ret = read_snappy_data(source, in_size, dest, out_size);
            break;
        default:
            throw FS_ARCHIVAL_ERROR("Unsupported compression type. [type=%d]", type);
    }
    POSTCONDITION(ret > 0);
}

int com::wookler::reactfs::core::archive_reader::read_zlib_data(void *source, uint64_t in_size, void *dest,
                                                                uint64_t out_size) {
    BYTE *c_ptr = static_cast<BYTE *>(source);

    z_stream _z_stream = {0};
    _z_stream.total_in = _z_stream.avail_in = in_size;
    _z_stream.total_out = _z_stream.avail_out = out_size;
    _z_stream.next_in = static_cast<Bytef *>(source);
    _z_stream.next_out = static_cast<Bytef *>(dest);

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

int com::wookler::reactfs::core::archive_reader::read_lzo_data(void *source, uint64_t in_size, void *dest,
                                                               uint64_t out_size) {
    return -1;
}

int com::wookler::reactfs::core::archive_reader::read_snappy_data(void *source, uint64_t in_size, void *dest,
                                                                  uint64_t out_size) {
    return -1;
}