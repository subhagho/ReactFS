//
// Created by Subhabrata Ghosh on 19/11/16.
//

#include <iostream>
#include "core/includes/common_structs.h"
#include "core/includes/server/node_init_manager.h"

using namespace com::wookler::reactfs::core;
typedef struct __record_index_header__ {
    char block_uid[SIZE_UUID];
    PADCHAR(0, 4);
    uint64_t block_id;
    uint64_t create_time = 0;
    uint64_t update_time = 0;
    uint64_t start_index = 0;
    uint64_t last_index = 0;
    //PADCHAR(1, 4);
    __write_state write_state = __write_state::WRITABLE;
    uint32_t total_size = 0;
    uint32_t used_size = 0;
    uint32_t write_offset = 0;
} __r_index_header;


int main(int argc, char *argv[]) {
    string s = common_utils::uuid();
    std::cout << "UUID : " << s.length() << "\n";
    std::cout << "BLOCK H SIZE=" << sizeof(__block_header) << "\n";
    std::cout << "INDEX H SIZE=" << sizeof(__r_index_header) << "\n";
    std::cout << "REC H SIZE=" << sizeof(__record_header) << "\n";

    std::cout << "uint64_t SIZE=" << sizeof(uint64_t) << "\n";
    std::cout << "uint32_t SIZE=" << sizeof(uint32_t) << "\n";
}
