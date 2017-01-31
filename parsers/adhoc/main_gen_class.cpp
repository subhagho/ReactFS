#include <iostream>
#include <cstring>
#include <stdlib.h>

#include "common/includes/init_utils.h"
#include "schema_driver.h"
#include "code_generator_cpp.h"
#include "parsers/test/test_type_data.h"

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

            vector<mutable_test_schema *> source;

            uint32_t rec_size = schema->estimate_size() * 200 * 5;
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
            uint64_t offset = 0;
            for (int ii = 0; ii < 10; ii++) {
                mutable_test_schema *ts = source[ii];
                mutable_record_struct *data = ts->serialize();
                CHECK_NOT_NULL(data);
                uint64_t w_size = writer->write(buffer, data, offset, rec_size);
                LOG_DEBUG("[%s] size=%lu", ts->get_key(), w_size);
                offset += w_size;
                CHECK_AND_FREE(data);


            }
            offset = 0;
            for (int ii = 0; ii < 10; ii++) {
                record_struct *rec = nullptr;
                uint64_t r_size = reader->read(buffer, &rec, offset, rec_size);
                CHECK_NOT_NULL(rec);
                test_schema *rs = new test_schema();
                CHECK_ALLOC(rs, TYPE_NAME(test_schema));
                rs->deserialize(rec);
                LOG_DEBUG("[%s] size=%lu", rs->get_key(), r_size);
                mutable_test_schema *ts = source[ii];
                POSTCONDITION(compare_test_schema::equals(rs, ts));
                CHECK_AND_FREE(rec);
                CHECK_AND_FREE(rs);
                offset += r_size;
            }
            FREE_PTR(buffer);

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
