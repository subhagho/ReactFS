#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

#include "common/includes/init_utils.h"
#include "types/includes/type_defs.h"
#include "parsers/includes/code_generator_cpp.h"
#include "parsers/includes/schema_driver.h"
#include "generator_util.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;
using namespace REACTFS_NS_CORE_PREFIX::parsers;

void generate_cpp(__complex_type *schema, const string &name_space, string &outdir, __version_header *version,
                  bool overwrite) {
    code_generator_cpp generator_cpp(outdir, overwrite);
    generator_cpp.generate_type_class(name_space, schema, schema->get_name(), *version);
}

void generate(const string &type, __complex_type *schema, const string &name_space, string &outdir,
              __version_header *version, bool overwrite) {
    CHECK_NOT_EMPTY(type);
    string t = string_utils::tolower(type);
    if (t == CPPT_CODE_TYPE) {
        generate_cpp(schema, name_space, outdir, version, overwrite);
    }
    throw BASE_ERROR("Un-supported code type. [type=%s]", t.c_str());
}

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

        if (options[GU_HELP] || argc == 0) {
            option::printUsage(std::cout, usage);
            return 0;
        }
        option::Option c = options[GU_CONFIG];
        if (c && c.count() > 0) {
            string configf = c.arg;
            CHECK_NOT_EMPTY(configf);
            env_utils::create_env(configf);
        } else {
            option::printUsage(std::cout, usage);
            return -1;
        }

        string schemafile;
        string outdir;
        string type;
        string version_s;
        bool overwrite = false;

        option::Option s = options[GU_SCHEMA_FILE];
        if (s && s.count() > 0) {
            schemafile = string(s.arg);
        }
        option::Option d = options[GU_OUTDIR];
        if (d && d.count() > 0) {
            outdir = string(d.arg);
        }
        option::Option t = options[GU_TYPE];
        if (t && t.count() > 0) {
            type = string(t.arg);
        }
        option::Option v = options[GU_VERSION];
        if (v && v.count() > 0) {
            version_s = string(v.arg);
        }
        option::Option o = options[GU_OVERWRITE];
        if (o) {
            overwrite = true;
        }
        CHECK_NOT_EMPTY(schemafile);
        CHECK_NOT_EMPTY(outdir);
        CHECK_NOT_EMPTY(type);
        CHECK_NOT_EMPTY(version_s);

        __version_header *version = common_utils::parse_version(version_s);
        CHECK_NOT_NULL(version);

        schema_driver driver;
        driver.parse(schemafile.c_str());

        __complex_type *schema = driver.translate();
        CHECK_NOT_NULL(schema);
        schema->print();

        generate(type, schema, driver.get_namespace(), outdir, version, overwrite);

        FREE_PTR(version);
        env_utils::dispose();
    } catch (const exception &e) {
        LOG_ERROR("Error occurred. [error=%s]", e.what());
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
