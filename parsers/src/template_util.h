//
// Created by Subhabrata Ghosh on 23/12/16.
//

#ifndef REACTFS_TEMPLATE_UTIL_H
#define REACTFS_TEMPLATE_UTIL_H

#include "include/options_args.h"

enum optionIndex {
    TU_UNKNOWN, TU_HELP, TU_CONFIG, TU_TEMPLATE_FILE, TU_OUTDIR
};
const option::Descriptor usage[] =
        {
                {optionIndex::TU_UNKNOWN, 0, "",  "",         option::Arg::None,     "USAGE: test_lock_client [options]\n\nOptions:"},
                {TU_HELP,                 0, "",  "help",     option::Arg::None,     "  --help  \tPrint usage and exit."},
                {TU_CONFIG,               0, "c", "config",   ExtendedArg::Required, "  --config=<filename>, -c <filename> \tPath to configuration file."},
                {TU_TEMPLATE_FILE,        0, "t", "template", ExtendedArg::Required, "  --template=<template file>, -p <template file> \tTemplate file to generate header for."},
                {TU_OUTDIR,               0, "d", "dir",      ExtendedArg::Required, "  --dir=<output directory>, -d <output directory> \tOutput directory to generate the header file."},
                {0,                       0, 0,   0,          0,                     0}
        };
#endif //REACTFS_TEMPLATE_UTIL_H
