//
// Created by subho on 6/1/17.
//

#ifndef REACTFS_TEST_TYPE_DATA_H
#define REACTFS_TEST_TYPE_DATA_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "parsers/test/generated/test_schema.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace com::wookler::test;

double DOUBLE_VALUES[] = {2120, 1293, 98439, 980202};

mutable_test_type *generate_type(const __complex_type *type, int index) {
    mutable_test_type *tt = new mutable_test_type(type);
    CHECK_ALLOC(tt, TYPE_NAME(mutable_test_type));

    string uuid = common_utils::uuid();
    uuid = common_utils::format("TEST_STRING_%s", uuid.c_str());
    tt->set_testString(uuid);

    int xi = index % 4;
    double dv = DOUBLE_VALUES[xi];

    tt->set_testDouble(dv);

    float fv = 1000 + (index * 1.005f);
    tt->set_testFloat(fv);

    for (int ii = 1; ii < index + 1; ii++) {
        string key = common_utils::format("TEST_STRING_%d::%d", index, ii);
        double dk = (index * 123.456f) / ii;
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

    for (int ii = 1; ii < index; ii++) {
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

    for (int ii = 1; ii < index; ii++) {
        mutable_test_type *tt = generate_type(lreft, ii + 1);
        ts->add_to_testListRef(tt);
    }
    return ts;
}
#endif //REACTFS_TEST_TYPE_DATA_H
