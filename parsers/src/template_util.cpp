#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

#include "common/includes/init_utils.h"
#include "types/includes/type_defs.h"
#include "parsers/includes/cpp_template.h"
#include "template_util.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;
using namespace REACTFS_NS_CORE_PREFIX::parsers;

int main(int argc, const char **argv) {
    try {
        argc -= (argc > 0);
        argv += (argc > 0); // skip program name argv[0] if present
        option::Stats stats(usage, argc, argv);
        std::vector<option::Option> options(stats.options_max);
        std::vector<option::Option> buffer(stats.buffer_max);
        option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

        if (parse.error())
            return 1;

        if (options[TU_HELP] || argc == 0) {
            option::printUsage(std::cout, usage);
            return 0;
        }
        option::Option o = options[TU_CONFIG];
        if (o && o.count() > 0) {
            string configf = o.arg;
            CHECK_NOT_EMPTY(configf);
            env_utils::create_env(configf);
        } else {
            option::printUsage(std::cout, usage);
            return -1;
        }

        string tempfile;
        string outdir;

        option::Option t = options[TU_TEMPLATE_FILE];
        if (t && t.count() > 0) {
            tempfile = string(t.arg);
        }
        option::Option d = options[TU_OUTDIR];
        if (d && d.count() > 0) {
            outdir = string(d.arg);
        }
        CHECK_NOT_EMPTY(tempfile);
        CHECK_NOT_EMPTY(outdir);

        cpp_template generator;
        generator.read_template(tempfile);
        generator.write_template_header(outdir);


        env_utils::dispose();
    } catch (const exception &e) {
        LOG_ERROR("Error occurred. [error=%s]", e.what());
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
