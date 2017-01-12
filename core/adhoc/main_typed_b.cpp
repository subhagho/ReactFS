//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include <stdlib.h>

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

#define DEFAULT_BLOCK_SIZE 1024 * 1024 * 512
#define RECORD_START_INDEX 100000
#define COUNT_RECORDS 500

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;
using namespace com::wookler::reactfs::core::types;
using namespace com::wookler::reactfs::core::parsers;

uint64_t write_data(typed_block *block, __complex_type *schema, int count, vector<int> *indexes, string &txid) {
    timer tw;
    mutable_test_schema *ts = generate_schema(schema, 10);
    CHECK_NOT_NULL(ts);
    for(int ii=0; ii < count; ii++) {
        tw.restart();
        mutable_record_struct *dt = ts->serialize();
        int index = block->write(dt, 0, txid);
        tw.pause();
        POSTCONDITION(index >= RECORD_START_INDEX);
        indexes->push_back(index);
        CHECK_AND_FREE(dt);
    }
    CHECK_AND_FREE(ts);
    return tw.get_elapsed();
}

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
    block->open(REUSE_BLOCK_INDEX_ID, p.get_path(), true);
    POSTCONDITION(block->get_block_state() == __state_enum::Available);
    vector<int> indexes;
    timer tw, tr;
    tw.start();
    string txid = block->start_transaction(0);
    uint64_t tt = 0;
    CHECK_NOT_EMPTY(txid);
    for(int ii=0; ii < 20; ii++) {
        tt += write_data(block, schema, 2000, &indexes, txid);
    }
    block->commit(txid);
    tw.stop();

    LOG_INFO("Written [%d] records to block in %d msec. [write time=%d]", indexes.size(), tw.get_elapsed(), tt);

    CHECK_AND_FREE(block);
    block = new typed_block();
    block->open(REUSE_BLOCK_INDEX_ID, p.get_path(), false);
    POSTCONDITION(block->get_block_state() == __state_enum::Available);

    vector<record_struct *> *records = new vector<record_struct *>();
    CHECK_ALLOC(records, TYPE_NAME(vector));
    for(int index : indexes) {
        tr.restart();
        int c = block->read_struct(index, 1, records);
        POSTCONDITION(records->size() > 0);
        record_struct *p0 = (*records)[0];
        test_schema *nts = new test_schema();
        CHECK_ALLOC(nts, TYPE_NAME(test_schema));
        nts->deserialize(p0);
        tr.pause();
        CHECK_AND_FREE(nts);
    }
    tr.stop();
    LOG_INFO("Read [%d] records from block in %d msec.", records->size(), tr.get_elapsed());
    for(auto vp : *records) {
        CHECK_AND_FREE(vp);
    }
    CHECK_AND_FREE(records);
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