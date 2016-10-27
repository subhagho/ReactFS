//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include "common/includes/log_utils.h"
#include "core/includes/fs_block.h"
#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "core/includes/fs_typed_block.h"

#define REUSE_BLOCK_FILE "/tmp/block_reused.basic"
#define REUSE_BLOCK_ID 1024
#define REUSE_BLOCK_TYPED_FILE "/tmp/block_reused.typed"
#define REUSE_TYPED_BLOCK_ID 1025

#define DEFAULT_BLOCK_SIZE 512

#define COUNT_TYPED 5

using namespace com::watergate::common;
using namespace com::wookler::reactfs::core;

typedef struct {
    uint64_t timestamp;
    char value[64];
    uint32_t index;
} _test_typed;

int main(int argc, char **argv) {
    try {
        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        fs_block *block = new fs_block();
        block->create(REUSE_BLOCK_ID, REUSE_BLOCK_FILE, DEFAULT_BLOCK_SIZE, true);

        block->open(REUSE_BLOCK_ID, REUSE_BLOCK_FILE);
        const char *dptr = "Returns true if the stream is currently associated with a file, and false otherwise.\n";

        for (int ii = 0; ii < 5; ii++) {
            block->write(dptr, strlen(dptr));
        }
        const void *rptr = block->read(256, 50);
        CHECK_NOT_NULL(rptr);

        char read[51];
        memset(read, 0, 51);
        memcpy(read, rptr, 50);

        LOG_DEBUG("Data read from file. [data=%s]", read);

        delete (block);

        fs_typed_block<_test_typed> *typed_block = new fs_typed_block<_test_typed>();
        typed_block->create(REUSE_TYPED_BLOCK_ID, REUSE_BLOCK_TYPED_FILE, DEFAULT_BLOCK_SIZE, true);

        typed_block->open(REUSE_TYPED_BLOCK_ID, REUSE_BLOCK_TYPED_FILE);

        _test_typed **ptr = (_test_typed **) malloc(COUNT_TYPED * sizeof(_test_typed **));
        uint64_t now = time_utils::now();

        for (int ii = 0; ii < COUNT_TYPED; ii++) {
            ptr[ii] = (_test_typed *) malloc(sizeof(_test_typed));
            memset(ptr[ii], 0, sizeof(_test_typed));

            ptr[ii]->index = ii;
            ptr[ii]->timestamp = now + (ii * 1000);
            string value = common_utils::format("THIS IS RECORD NUMBER %d", ii);
            strncpy(ptr[ii]->value, value.c_str(), value.length());
        }

        row<_test_typed> **written = typed_block->write_t(ptr, COUNT_TYPED);
        CHECK_NOT_NULL(written);
        for (int ii = 0; ii < COUNT_TYPED; ii++) {
            const uint64_t rid = written[ii]->get_rowid();
            const _test_typed *t = written[ii]->get_data();
            LOG_DEBUG("record[rowid=%lu] : index=%d; timestamp=%lu; value=%s", rid, t->index, t->timestamp, t->value);
        }
        delete (written);

        LOG_DEBUG("BLOCK USED SPACE = %lu", typed_block->get_used_space());
        LOG_DEBUG("BLOCK AVAILABLE SPACE = %lu", typed_block->get_free_space());

        row<_test_typed> *r = nullptr;

        for (int ii = 0; ii < COUNT_TYPED; ii++) {
            r = typed_block->read_t(ii);
            const uint64_t rid = r->get_rowid();
            const _test_typed *t = r->get_data();

            LOG_DEBUG("record[rowid=%lu] : index=%d; timestamp=%lu; value=%s", rid, t->index, t->timestamp, t->value);
        }

        delete (typed_block);

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}