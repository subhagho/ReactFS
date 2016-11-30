/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//
// Created by Subhabrata Ghosh on 29/11/16.
//

#ifndef REACTFS_USAGE_METRIC_DEFS_H
#define REACTFS_USAGE_METRIC_DEFS_H
namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                typedef struct __usage_array__ {
                    uint16_t index;
                    uint64_t value;
                    uint64_t time;
                } __usage_array;

                template<uint16_t __SIZE__>
                struct __usage_metric__ {
                    uint16_t size = __SIZE__;
                    uint64_t reset_time;
                    uint64_t total_value;
                    uint64_t total_time;
                    uint16_t current_index;
                    __usage_array records[__SIZE__];
                };

                typedef __usage_metric__<24> __hourly_usage_metric;

            }
        }
    }
}
#endif //REACTFS_USAGE_METRIC_DEFS_H
