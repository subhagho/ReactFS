//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include <cstdlib>

#include "common/includes/log_utils.h"
#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "core/includes/base_block.h"
#include "core/includes/base_indexed_block.h"
#include "core/includes/block_utils.h"
#include "core/includes/node_env.h"
#include "common/includes/shared_lock_utils.h"
#include "common/includes/init_utils.h"
#include "parsers/includes/schema_driver.h"
#include "parsers/test/test_type_data.h"

#define REUSE_PARENT_ID ULONG_MAX

#define REUSE_BLOCK_INDEX_FILE "block_reused.typed"
#define REUSE_BLOCK_INDEX_ID (ULONG_MAX - 1)

#define REUSE_BLOCK_FILE_COMP "/tmp/block_reused.comp"
#define REUSE_BLOCK_ID_COMP 1025

#define DEFAULT_BLOCK_SIZE 1024 * 1024 * 32
#define RECORD_START_INDEX 100000
#define COUNT_RECORDS 500

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;
using namespace com::wookler::reactfs::core::types;
using namespace com::wookler::reactfs::core::parsers;

void test_indexed(char *schemaf) {
    node_client_env *c_env = node_init_client::get_client_env();

    schema_driver driver;
    driver.parse(schemaf);

    __complex_type *schema = driver.translate();
    CHECK_NOT_NULL(schema);
    schema->print();

    uint32_t rec_size = schema->estimate_size();
    LOG_DEBUG("Estimated record size = %d", rec_size);

    string s = block_utils::get_block_dir(c_env->get_mount_client(), DEFAULT_BLOCK_SIZE);
    Path p(s);
    p.append(REUSE_BLOCK_INDEX_FILE);

    LOG_DEBUG("Using block file [%s]", p.get_path().c_str());

    string uuid = block_utils::create_typed_block(schema, REUSE_BLOCK_INDEX_ID, REUSE_PARENT_ID, p.get_path(),
                                                  __block_def::INDEXED,
                                                  __block_usage::PRIMARY,
                                                  DEFAULT_BLOCK_SIZE,
                                                  rec_size, RECORD_START_INDEX, true);
    POSTCONDITION(!IS_EMPTY(uuid));

    typed_block *block = new typed_block();
    CHECK_ALLOC(block, TYPE_NAME(typed_block));
    block->open(REUSE_BLOCK_INDEX_ID, p.get_path());
    POSTCONDITION(block->get_block_state() == __state_enum::Available);
    string txid = block->start_transaction(0);
    mutable_test_schema *ts = generate_schema(schema, 100);
    CHECK_NOT_NULL(ts);
    mutable_record_struct *dt = ts->serialize();
    int index = block->write(dt, 0, txid);
    block->commit(txid);

    vector<record_struct *> records;
    int c = block->read_struct(index, 1, &records);
    POSTCONDITION(c > 0);
    record_struct *p0 = records[0];
    test_schema * nts = new test_schema();
    nts->deserialize(p0);
    CHECK_NOT_NULL(nts);
    CHECK_AND_FREE(block);
    
}


int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 2);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);
        char *sf = argv[2];
        CHECK_NOT_NULL(sf);

        string configf(cf);

        node_init_client::create_node_env(configf);
        node_client_env *c_env = node_init_client::get_client_env();
        CHECK_NOT_NULL(c_env);

        test_indexed(sf);

        node_init_client::shutdown();

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}