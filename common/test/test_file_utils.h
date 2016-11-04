/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
//
// Created by Subhabrata Ghosh on 20/09/16.
//

#ifndef WATERGATE_TEST_ALARM_H
#define WATERGATE_TEST_ALARM_H

#define CATCH_CONFIG_MAIN

#include <iostream>

#include "test_common.h"
#include "common/includes/timer.h"
#include "common/includes/__alarm.h"
#include "common/includes/init_utils.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")

using namespace com::watergate::common;

#define COPY_FILE_PATH "common/test/data/test-conf.json"
#define COPY_DEST_PATH "test/copy"

#endif //WATERGATE_TEST_ALARM_H
