//
// Created by Subhabrata Ghosh on 23/12/16.
//

#ifndef REACTFS_TEMPLATE_UTIL_H
#define REACTFS_TEMPLATE_UTIL_H

#include "include/options_args.h"

#define DEFAULT_STYLE_OPTS "-A10tOPxnN"

enum optionIndex {
    GU_UNKNOWN, GU_HELP, GU_CONFIG, GU_SCHEMA_FILE, GU_OUTDIR, GU_TYPE, GU_VERSION, GU_OVERWRITE, GU_STYLE_OPT
};
const option::Descriptor usage[] =
        {
                {optionIndex::GU_UNKNOWN, 0, "",  "",          option::Arg::None,     "USAGE: test_lock_client [options]\n\nOptions:"},
                {GU_HELP,                 0, "",  "help",      option::Arg::None,     "  --help  \tPrint usage and exit."},
                {GU_STYLE_OPT,            0, "",  "style",     option::Arg::None,     "  --style  \tOptions to be passed to ASTYLE code formatter."},
                {GU_OVERWRITE,            0, "o", "overwrite", option::Arg::None,     "  --overwrite, -o  \tOverwrite all generated files."},
                {GU_CONFIG,               0, "c", "config",    ExtendedArg::Required, "  --config=<filename>, -c <filename> \tPath to configuration file."},
                {GU_SCHEMA_FILE,          0, "s", "schema",    ExtendedArg::Required, "  --schema=<schema file>, -s <schema file> \tSchema file to generate classes for."},
                {GU_OUTDIR,               0, "d", "dir",       ExtendedArg::Required, "  --dir=<output directory>, -d <output directory> \tOutput directory to generate the file(s)."},
                {GU_TYPE,                 0, "t", "type",      ExtendedArg::Required, "  --type=<cpp/java>, -t <cpp/java> \tType of code to generate [cpp, java]"},
                {GU_VERSION,              0, "v", "version",   ExtendedArg::Required, "  --version=<schema version>, -v <schema version> \tVersion of the schema file."},
                {0,                       0, 0,   0,           0,                     0}
        };
#endif //REACTFS_TEMPLATE_UTIL_H
