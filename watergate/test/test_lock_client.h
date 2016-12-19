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
// Created by Subhabrata Ghosh on 19/09/16.
//

#ifndef WATERGATE_TEST_LOCK_CLIENT_H
#define WATERGATE_TEST_LOCK_CLIENT_H

#include <unistd.h>

#include "common/includes/__env.h"
#include "watergate/includes/control_def.h"
#include "watergate/includes/init_utils.h"

#include "include/options_args.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")
#define CONTROL_DEF_CONFIG_PATH "/configuration/control"
#define CONTROL_CONFIG_PATH "/configuration/control"


#define TLC_CONFIG_NODE "/configuration/test-lock-client"
#define TCL_CONFIG_VALUE_PROCESS "process"
#define TCL_CONFIG_VALUE_PROCESSES "process.count"
#define TCL_CONFIG_VALUE_SLEEP "sleep.time"
#define TCL_CONFIG_VALUE_TRIES "lock.tries"

#define CONTROL_NAME "dummy-resource-1"
#define FS_CONTROL_NAME "/private/tmp/"

using namespace com::wookler::watergate::core;
using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace tests {
                namespace common {
                    class basic_lock_client {
                    private:
                        int priority;
                        long sleep_timeout;
                        int lock_tries;

                    public:
                        basic_lock_client(int priority) {
                            this->priority = priority;
                        }

                        void setup();

                        void run();
                    };
                }
            }
        }
    }
}

enum optionIndex {
    UNKNOWN, HELP, CONFIG, PRIORITY, INDEX
};
const option::Descriptor usage[] =
        {
                {optionIndex::UNKNOWN, 0, "",  "",         option::Arg::None,     "USAGE: test_lock_client [options]\n\nOptions:"},
                {HELP,                 0, "",  "help",     option::Arg::None,     "  --help  \tPrint usage and exit."},
                {CONFIG,               0, "c", "config",   ExtendedArg::Required, "  --config=<filename>, -c <filename> \tPath to configuration file."},
                {PRIORITY,             0, "p", "priority", ExtendedArg::Numeric,  "  --priority=<priority>, -p <priority> \tPriority to acquire locks."},
                {INDEX,                0, "i", "index",    ExtendedArg::Numeric,  "  --index=<index>, -p <index> \tIndex of the launched instance."},
                {0,                    0, 0,   0,          0,                     0}
        };

#endif //WATERGATE_TEST_LOCK_CLIENT_H
