//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include "common/includes/log_utils.h"
#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "core/includes/base_block.h"

#define REUSE_BLOCK_FILE "/tmp/block_reused.raw"
#define REUSE_BLOCK_ID 1024

#define REUSE_BLOCK_FILE_COMP "/tmp/block_reused.comp"
#define REUSE_BLOCK_ID_COMP 1025

#define DEFAULT_BLOCK_SIZE 1024

#define COUNT_RECORDS 50

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

typedef struct {
    uint64_t timestamp;
    char value[128];
    uint32_t index;
} _test_typed;

void test_raw() {
    base_block *block = new base_block();
    block->create(REUSE_BLOCK_ID, REUSE_BLOCK_FILE, __block_type::PRIMARY, DEFAULT_BLOCK_SIZE, true);
    CHECK_AND_FREE(block);

    block = new base_block();
    block->open(REUSE_BLOCK_ID, REUSE_BLOCK_FILE);
    POSTCONDITION(block->get_block_state() == __state_enum::Available);

    string txid = block->start_transaction();
    POSTCONDITION(!IS_EMPTY(txid));
    for (int ii = 0; ii < COUNT_RECORDS; ii++) {
        _test_typed t;
        t.index = ii;
        t.timestamp = time_utils::now() + (ii * 10000);
        string uuid = common_utils::uuid();
        strncpy(t.value, uuid.c_str(), uuid.length());

        block->write(&t, sizeof(_test_typed), txid);
    }
    block->commit(txid);
}

void test_compressed() {

}

int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        test_raw();

        test_compressed();

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}