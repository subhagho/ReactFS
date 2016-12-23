//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/base_error.h"

using namespace std;
using namespace com::wookler::reactfs::common;

string set_token(const string &token, string &value, string &input) {
    uint32_t offset = 0;
    stringstream ss;
    while (offset < input.length()) {
        size_t index = input.find(token, offset);
        if (index == string::npos) {
            string str = input.substr(offset, input.length());
            ss << str;
            break;
        }
        string::size_type len = (index - offset);
        string str = input.substr(offset, len);
        ss << str;
        ss << value;
        offset += (index + token.length());
    }
    return string(ss.str());
}

int main(int argc, char *argv[]) {
    string str = "This is a ${string} that has \"${tokens}\" in the ${string}";
    //str = string_utils::unescape(str);
    string ts = "STRING";
    string tt = "TOKENS";
    string ns = set_token("${string}", ts, str);
    ns = set_token("${tokens}", tt, ns);

    cout << "INPUT STRING [" << str << "]\n";
    cout << "OUTPUT STRING [" << ns << "]\n";
}