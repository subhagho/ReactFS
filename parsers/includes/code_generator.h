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

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class code_generator {
                    protected:
                        Path *outpath = nullptr;

                    public:
                        virtual ~code_generator() {
                            CHECK_AND_FREE(outpath);
                        }

                        virtual string generate_type_class(__reference_type *type) = 0;

                        virtual string generate_schema_class(__schema_def *schema,
                                                             unordered_map<string, __reference_type *> *types) = 0;
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CODE_GENERATOR_H
