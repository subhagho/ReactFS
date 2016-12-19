//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/base_error.h"

using namespace std;
using namespace com::wookler::reactfs::common;


int main(int argc, char *argv[]) {
   string s1 = common_utils::uuid();
   string s2 = common_utils::uuid();
   string s3 = common_utils::uuid();

   string h1 = common_utils::get_name_hash(s1);
   cout << "Name hash [" << s1 << ":" << h1 << "\n";

   string h2 = common_utils::get_name_hash(s2);
   cout << "Name hash [" << s2 << ":" << h2 << "\n";

   string h3 = common_utils::get_name_hash(s3);
   cout << "Name hash [" << s3 << ":" << h3 << "\n";
}