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


test_type *generate_type(int index) {
    test_type *tt = new test_type();
    CHECK_ALLOC(tt, TYPE_NAME(test_type));

    string uuid = common_utils::uuid();
    uuid = common_utils::format("TEST_STRING_%s", uuid.c_str());
    tt->set_testString(uuid);

    double dv = index * 123.456f;
    tt->set_testDouble(dv);

    float fv = index * 1.005f;
    tt->set_testFloat(fv);

    for (int ii = 0; ii < index; ii++) {
        string key = common_utils::format("TEST_STRING_%d::%d", index, ii);
        double dk = (index * 123.456f) / ii;
        tt->add_to_testListString(key);
        tt->add_to_testMapString(dk, key);
    }
    return tt;
}

test_ref_type *generate_ref(string &key, int index) {
    test_ref_type *tr = new test_ref_type();
    CHECK_ALLOC(tr, TYPE_NAME(test_ref_type));

    tr->set_name(key);
    for (int ii = 0; ii < index; ii++) {
        string key = common_utils::format("TEST_REF_TYPE_%d::%d", index, ii);
        test_type *tt = generate_type(ii);
        tr->add_to_testRefMap(key, tt);
    }
    return tr;
}

test_schema *generate_schema(int index) {
    test_schema *ts = new test_schema();
    CHECK_ALLOC(ts, TYPE_NAME(test_schema));

    string uuid = common_utils::uuid();
    ts->set_key(uuid);

    short sv = (short) (index % 1024);
    ts->set_shortWithDefault(sv);
    long lv = index * 1024;
    ts->set_testLong(lv);

    test_ref_type *tr = generate_ref(uuid, index);
    ts->set_testTypeRef(tr);

    for (int ii = 0; ii < index; ii++) {
        test_type *tt = generate_type(ii);
        ts->add_to_testListRef(tt);
    }
    return ts;
}

int
main(const int argc, const char **argv) {
    /** check for the right # of arguments **/
    if (argc == 2) {
        try {
            const char *cf = argv[1];
            CHECK_NOT_NULL(cf);

            string configf(cf);
            env_utils::create_env(configf);

            vector<test_schema *> source;
            vector<test_schema *> target;

            for (int ii = 10; ii < 20; ii++) {
                test_schema *ts = generate_schema(ii);
                source.push_back(ts);
            }

            for (int ii = 0; ii < 10; ii++) {
                test_schema *ts = source[ii];
                __struct_datatype__ *data = ts->serialize();
                CHECK_NOT_NULL(data);
                ts->free_data_ptr(data, false);

                test_schema *tsn = new test_schema(*ts);
                target.push_back(tsn);
            }

            for (int ii = 0; ii < 10; ii++) {
                test_schema *ts = source[ii];
                CHECK_AND_FREE(ts);

                test_schema *tsn = target[ii];
                CHECK_AND_FREE(tsn);
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
