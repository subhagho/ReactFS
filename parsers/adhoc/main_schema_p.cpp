#include <iostream>
#include <cstdlib>
#include <cstring>

#include "common/includes/init_utils.h"
#include "schema_driver.h"
#include "code_generator_cpp.h"

using namespace REACTFS_NS_COMMON_PREFIX;

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
            schema->print();

            unordered_map<string, __native_type *> fields;
            schema->get_field_index(&fields);

            for (auto iter = fields.begin(); iter != fields.end(); iter++) {
                LOG_DEBUG("FIELD : [%s]", iter->first.c_str());
            }
            vector<record_index *> *indexes = driver.get_indexes(schema);
            CHECK_NOT_EMPTY_P(indexes);
            for(record_index *ri : *indexes) {
                CHECK_AND_FREE(ri);
            }
            CHECK_AND_FREE(indexes);
            CHECK_AND_FREE(schema);

            env_utils::dispose();
        } catch (const exception &e) {
            LOG_ERROR("Error occurred. [error=%s]", e.what());
            return (EXIT_FAILURE);
        }
    } else {
        cout << "Usage: " << argv[0] << " <config file> <schema file> \n";
        /** exit with failure condition **/
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
