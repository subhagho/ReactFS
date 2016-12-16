#include <iostream>
#include <cstdlib>
#include <cstring>

#include "common/includes/init_utils.h"
#include "schema_driver.h"

using namespace REACTFS_NS_COMMON_PREFIX;

int
main(const int argc, const char **argv) {
    /** check for the right # of arguments **/
    if (argc == 3) {
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

        env_utils::dispose();
    } else {
        cout << "Usage: " << argv[0] << " <config file> <schema file> \n";
        /** exit with failure condition **/
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
