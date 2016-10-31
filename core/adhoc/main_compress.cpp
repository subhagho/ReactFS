//
// Created by Subhabrata Ghosh on 29/10/16.
//

#include <zlib.h>

#define COMPRESS_BLOCK_SIZE 128 * 1024

#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "common/includes/log_utils.h"


int main(int argc, char **argv) {
    try {
        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        const char *dptr = "Returns true if the stream is currently associated with a file, and false otherwise.\n";
        const int size = strlen(dptr);

        char *input = (char *) malloc((size + 1) * 256 * sizeof(char));
        memset(input, 0, (size * 256 * sizeof(char)));
        int offset = 0;
        for (int ii = 0; ii < 256; ii++) {
            offset = (size * ii);
            char *ptr = input + offset;
            memcpy(ptr, dptr, size);
        }

        LOG_DEBUG("SOURCE [%s]", input);

        char *ptr = (char *) malloc((size + 1) * 256 * sizeof(char));
        memset(ptr, 0, (size * 256 * sizeof(char)));
        memcpy(ptr, input, (size * 256 * sizeof(char)));
        memset(input, 0, (size * 256 * sizeof(char)));

        CHECK_NOT_NULL(ptr);

        std::vector<uint8_t> buffer;

        uint64_t data_size = (size + 1) * 256 * sizeof(char);
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

        LOG_DEBUG("Compressed data size = %d", buffer.size());
       
        uint8_t *data = buffer.data();

        _z_stream = {0};
        _z_stream.total_in = _z_stream.avail_in = buffer.size();
        _z_stream.total_out = _z_stream.avail_out = data_size;
        _z_stream.next_in = reinterpret_cast<Bytef *>(data);
        _z_stream.next_out = reinterpret_cast<Bytef *>(input);

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

        LOG_DEBUG("OUTPUT [%s]", input);

        env_utils::dispose();

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}