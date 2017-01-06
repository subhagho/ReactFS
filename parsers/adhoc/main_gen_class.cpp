#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

#include "common/includes/init_utils.h"
#include "schema_driver.h"
#include "code_generator_cpp.h"
#include "parsers/test/generated/test_schema.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace com::wookler::test;


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

int
main(const int argc, const char **argv) {
    /** check for the right # of arguments **/
    if (argc == 3) {
        try {
            const char *cf = argv[1];
            CHECK_NOT_NULL(cf);

            string configf(cf);
            env_utils::create_env(configf);

            const char *sf = argv[2];
            CHECK_NOT_NULL(sf);

            com::wookler::reactfs::core::parsers::schema_driver driver;
            driver.parse(sf);

            __complex_type *schema = driver.translate();
            CHECK_NOT_NULL(schema);

            vector<mutable_test_schema *> source;

            uint32_t rec_size = schema->estimate_size() * 20;
            LOG_DEBUG("Estimated record size = %d", rec_size);

            for (int ii = 10; ii < 20; ii++) {
                mutable_test_schema *ts = generate_schema(schema, ii);
                source.push_back(ts);
            }

            __base_datatype_io *writer = schema->get_type_handler(__record_mode::RM_WRITE);
            CHECK_NOT_NULL(writer);

            __base_datatype_io *reader = schema->get_type_handler(__record_mode::RM_READ);
            CHECK_NOT_NULL(reader);

            void *buffer = malloc(sizeof(char) * rec_size);
            for (int ii = 0; ii < 10; ii++) {
                mutable_test_schema *ts = source[ii];
                mutable_record_struct *data = ts->serialize();
                CHECK_NOT_NULL(data);
                // data->print();
                memset(buffer, 0, rec_size);
                writer->write(buffer, data, 0, rec_size);
                CHECK_AND_FREE(data);

                record_struct *rec = nullptr;
                reader->read(buffer, &rec, 0, rec_size);
                CHECK_NOT_NULL(rec);
            }

            for (int ii = 0; ii < 10; ii++) {
                mutable_test_schema *ts = source[ii];
                CHECK_AND_FREE(ts);
            }
            env_utils::dispose();
        } catch (const exception &e) {
            LOG_ERROR("Error occurred. [error=%s]", e.what());
            return (EXIT_FAILURE);
        }
    } else {
        cout << "Usage: " << argv[0] << " <config file> \n";
        /** exit with failure condition **/
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
