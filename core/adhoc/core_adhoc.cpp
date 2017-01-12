//
// Created by Subhabrata Ghosh on 19/11/16.
//

#include <iostream>
#include "core/includes/common_structs.h"
#include "core/includes/server/node_init_manager.h"
#include "common/includes/timer.h"
#include <openssl/md5.h>

using namespace com::wookler::reactfs::core;


int main(int argc, char *argv[]) {
    timer t;
    t.start();
    uint32_t count = 100000;
    for (uint32_t ii = 0; ii < count; ii++) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        string u1 = common_utils::uuid();
        string u2 = common_utils::uuid();
        string ss = common_utils::format("%s%s", u1.c_str(), u2.c_str());
        MD5((unsigned char *) ss.c_str(), ss.length(), (unsigned char *) &digest);
    }
    t.stop();
    cout << "[Run count = " << count << "] elapsed time = " << t.get_elapsed() << "\n";
}
