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

string get_next_token(string &input, uint32_t *offset) {
    string ts("${");
    string te("}");

    size_t index = input.find(ts, *offset);
    if (index != string::npos) {
        size_t endi = input.find(te, index);
        if (endi == string::npos) {
            throw BASE_ERROR("Invalid token definition. Token terminator missing.");
        }
        string::size_type len = (endi - index + 1);
        string token = input.substr(index, len);
        CHECK_NOT_EMPTY(token);
        *offset = endi + 1;
        return token;
    }
    return common_consts::EMPTY_STRING;
}

void extract_tokens(string &input) {
    uint32_t offset = 0;
    while (offset < input.length()) {
        string token = get_next_token(input, &offset);
        if (IS_EMPTY(token)) {
            break;
        }
        cout << "Found token [" << token << "\n";
    }
}

int main(int argc, char *argv[]) {
    string str = "        } else if (this->is_list(\"${type}\") || this->is_map(\"${type}\")) {}";
    extract_tokens(str);
}