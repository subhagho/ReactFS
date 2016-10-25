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
// Created by Subhabrata Ghosh on 16/10/16.
//

#ifndef WATERGATE_MAIN_FS_H_H
#define WATERGATE_MAIN_FS_H_H

#include <fstream>

#include "watergate/includes/control_def.h"
#include "watergate/includes/fs_writer.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")
#define CONTROL_DEF_CONFIG_PATH "/configuration/control/def"
#define CONTROL_CONFIG_PATH "/configuration/control"

#define TFW_CONFIG_NODE "/configuration/test-fs-writer"
#define TFW_CONFIG_VALUE_DATAF "data-file"

#define CONTROL_NAME "/private/tmp"

typedef struct {
    uint64_t timestamp;
    uint32_t index;
    string data;
} dummy_rec;

using namespace std;

string read_data_file(string filename) {
    cout << "Opening data file. [file=" << filename << "]\n";
    std::ifstream t(filename, std::ifstream::in);
    if (!t.is_open()) {
        throw CONFIG_ERROR("Error opening configuration file. [file=%s][error=%s]", filename.c_str(),
                           strerror(errno));
    }

    stringstream ss;
    string line;
    while (getline(t, line)) {
        ss << line;
    }
    t.close();

    string data(ss.str());

    CHECK_NOT_EMPTY(data);

    return data;
}

#endif //WATERGATE_MAIN_FS_H_H
