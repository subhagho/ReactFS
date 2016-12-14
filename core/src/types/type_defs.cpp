//
// Created by Subhabrata Ghosh on 03/12/16.
//

#include "core/includes/types/type_defs.h"

using namespace com::wookler::reactfs::core::types;

// Initialize the static type handlers map.
unordered_map<string, __base_datatype_io *> com::wookler::reactfs::core::types::__type_defs_utils::type_handlers;

// Initialize the static type handlers lock.
mutex com::wookler::reactfs::core::types::__type_defs_utils::thread_mutex;

// Set the type loader implementation.
__complex_type_helper *com::wookler::reactfs::core::types::__complex_type_helper::type_loader = new complex_type_loader_impl();
