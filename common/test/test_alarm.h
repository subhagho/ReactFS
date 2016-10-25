//
// Created by Subhabrata Ghosh on 20/09/16.
//

#ifndef WATERGATE_TEST_ALARM_H
#define WATERGATE_TEST_ALARM_H

#define CATCH_CONFIG_MAIN

#include <iostream>

#include "test_common.h"
#include "common/includes/timer.h"
#include "common/includes/alarm.h"
#include "common/includes/init_utils.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")

using namespace com::watergate::common;

class test_type {
private:
    string *value;
public:
    test_type(string *value) {
        this->value = value;
    }

    string get_value() {
        if (NOT_NULL(value)) {
            return *value;
        }
        return EMPTY_STRING;
    }
};

class test_callback : public com::watergate::common::__callback {
public:
    test_callback(test_type *value) {
        this->set_context(value);
    }

    void callback() override {
        test_type *p = nullptr;
        p = get_context(p);
        LOG_INFO("Callback context [value=%s]", p->get_value().c_str());
    }
};

#endif //WATERGATE_TEST_ALARM_H
