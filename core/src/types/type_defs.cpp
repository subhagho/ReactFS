//
// Created by Subhabrata Ghosh on 03/12/16.
//

#include "core/includes/types/type_defs.h"

using namespace com::wookler::reactfs::core::types;

unordered_map<string, __base_datatype_io *> com::wookler::reactfs::core::types::__type_defs_utils::type_handlers;
__complex_type_helper *com::wookler::reactfs::core::types::__complex_type_helper::type_loader = new complex_type_loader_impl();
