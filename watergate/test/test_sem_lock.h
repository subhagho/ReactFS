//
// Created by Subhabrata Ghosh on 16/09/16.
//

#ifndef WATERGATE_TEST_SEM_LOCK_H
#define WATERGATE_TEST_SEM_LOCK_H
#define CATCH_CONFIG_MAIN

#include <iostream>

#ifndef __APPLE__
#include <wait.h>
#endif

#include "include/catch.hpp"
#include "common/includes/__env.h"
#include "watergate/includes/control_def.h"
#include "common/test/test_common.h"

#endif //WATERGATE_TEST_SEM_LOCK_H
