//
// Created by Subhabrata Ghosh on 15/09/16.
//

#include "includes/core/resource_def.h"

com::watergate::core::resource_def::resource_def(resource_type_enum type) {
    this->type = type;
}

com::watergate::core::resource_type_enum com::watergate::core::resource_def::get_type() {
    return this->type;
}