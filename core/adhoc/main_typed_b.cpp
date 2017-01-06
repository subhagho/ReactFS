//
// Created by Subhabrata Ghosh on 27/10/16.
//

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
#include "parsers/test/generated/test_schema.h"

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
using namespace com::wookler::test;

double D_VALUES[] = {2120, 1293, 98439, 980202};


mutable_test_type *generate_type(const __complex_type *type, int index) {
    mutable_test_type *tt = new mutable_test_type(type);
    CHECK_ALLOC(tt, TYPE_NAME(mutable_test_type));

    string uuid = common_utils::uuid();
    uuid = common_utils::format("TEST_STRING_%s", uuid.c_str());
    tt->set_testString(uuid);

    double dv = index * 123.456f;
    tt->set_testDouble(dv);

    float fv = index * 1.005f;
    tt->set_testFloat(fv);

    for (int ii = 0; ii < index + 1; ii++) {
        string key = common_utils::format("TEST_STRING_%d::%d", index, ii);
        double dk = (index * 123.456f) / (ii + 1);
        tt->add_to_testListString(key);
        tt->add_to_testMapString(dk, key);
    }
    return tt;
}

mutable_test_ref_type *generate_ref(const __complex_type *type, string &key, int index) {
    mutable_test_ref_type *tr = new mutable_test_ref_type(type);
    CHECK_ALLOC(tr, TYPE_NAME(test_ref_type));

    tr->set_name(key);
    const __native_type *nt = type->get_field("testRefMap");
    CHECK_NOT_NULL(nt);
    const __map_type *mt = dynamic_cast<const __map_type *>(nt);
    CHECK_CAST(mt, TYPE_NAME(__native_type), TYPE_NAME(__map_type));
    nt = mt->get_value_type();
    CHECK_NOT_NULL(nt);
    const __complex_type *vt = dynamic_cast<const __complex_type *>(nt);
    CHECK_CAST(vt, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

    for (int ii = 0; ii < index; ii++) {
        string key = common_utils::format("TEST_REF_TYPE_%d::%d", index, ii);
        mutable_test_type *tt = generate_type(vt, ii + 1);
        tr->add_to_testRefMap(key, tt);
    }
    return tr;
}

mutable_test_schema *generate_schema(const __complex_type *schema, int index) {
    mutable_test_schema *ts = new mutable_test_schema(schema);
    CHECK_ALLOC(ts, TYPE_NAME(test_schema));

    string uuid = common_utils::uuid();
    ts->set_key(uuid);

    short sv = (short) (index % 1024);
    ts->set_shortWithDefault(sv);
    long lv = index * 1024;
    ts->set_testLong(lv);

    const __native_type *type = schema->get_field("testTypeRef");
    CHECK_NOT_NULL(type);
    const __complex_type *reft = dynamic_cast<const __complex_type *>(type);
    CHECK_CAST(reft, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

    mutable_test_ref_type *tr = generate_ref(reft, uuid, index);
    ts->set_testTypeRef(tr);

    type = schema->get_field("testListRef");
    CHECK_NOT_NULL(type);
    const __list_type *listRef = dynamic_cast<const __list_type *>(type);
    CHECK_CAST(listRef, TYPE_NAME(__native_type), TYPE_NAME(__list_type));
    type = listRef->get_inner_type();
    CHECK_NOT_NULL(type);
    const __complex_type *lreft = dynamic_cast<const __complex_type *>(type);
    CHECK_CAST(lreft, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

    for (int ii = 0; ii < index; ii++) {
        mutable_test_type *tt = generate_type(lreft, ii + 1);
        ts->add_to_testListRef(tt);
    }
    return ts;
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