//
// Created by Subhabrata Ghosh on 25/12/16.
//

#ifndef REACTFS_CPP_FILE_TEMPLATE_H
#define REACTFS_CPP_FILE_TEMPLATE_H

#include "core/includes/core.h"
#include "code_generator.h"
#include "cpp_template_header.h"
#include "cpp_template_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
    namespace parsers {
        class cpp_file_template {
        private:
            stringstream file_s;
            stringstream class_s;
            stringstream declare_s;
            stringstream pr_methods_s;
            stringstream pu_methods_s;

        public:

        };
    }
REACTFS_NS_CORE_END
#endif //REACTFS_CPP_FILE_TEMPLATE_H
