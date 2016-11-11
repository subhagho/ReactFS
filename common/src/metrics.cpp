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
// Created by Subhabrata Ghosh on 04/10/16.
//

#include "common/includes/metrics.h"

mutex com::wookler::reactfs::common::metrics_utils::g_lock;

__state__ com::wookler::reactfs::common::metrics_utils::state;

unordered_map<string, __metric *> *com::wookler::reactfs::common::metrics_utils::metrics = new unordered_map<string, __metric *>();