//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_CODE_GENERATOR_H
#define REACTFS_CODE_GENERATOR_H

#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"
#include "types/includes/type_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;

REACTFS_NS_CORE
                namespace parsers {
                    class code_generator {
                    protected:
                        Path *outpath = nullptr;
                        bool overwrite = true;

                    public:
                        virtual ~code_generator() {
                            CHECK_AND_FREE(outpath);
                        }

                        virtual string generate_type_class(const string &name_space, __complex_type *type) = 0;
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CODE_GENERATOR_H
