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

#ifndef WATERGATE_METRICS_H
#define WATERGATE_METRICS_H

#include <mutex>
#include <unordered_map>

#include "common.h"
#include "common_utils.h"
#include "timer.h"
#include "log_utils.h"
#include "__state__.h"

#define START_TIMER(n) timer _t_##n; \
     _t_##n.start();

#define END_TIMER(m, n) com::wookler::reactfs::common::metrics_utils::timed_metric(m, _t_##n);

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
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

                enum __metric_type_enum {
                    BasicMetric, AverageMetric
                };

                class __metric {
                protected:
                    string *name;
                    __metric_type_enum type;
                    double value;
                    bool thread_safe = false;
                    double max_value = 0;

                public:
                    __metric(string name) {
                        PRECONDITION(!IS_EMPTY(name));

                        this->type = BasicMetric;
                        this->name = new string(name);
                        this->value = 0;
                    }

                    __metric(string name, bool thread_safe) {
                        PRECONDITION(!IS_EMPTY(name));

                        this->type = BasicMetric;
                        this->name = new string(name);
                        this->value = 0;
                        this->thread_safe = thread_safe;
                    }

                    virtual ~__metric() {
                        if (NOT_NULL(name)) {
                            delete (name);

                            name = nullptr;
                        }
                    }

                    string *get_name() {
                        return this->name;
                    }

                    __metric_type_enum get_type() {
                        return this->type;
                    }

                    string get_type_string() {
                        switch (type) {
                            case BasicMetric:
                                return "BasicMetric";
                            case AverageMetric:
                                return "AverageMetric";
                        }
                    }

                    bool is_thread_safe() {
                        return this->thread_safe;
                    }

                    virtual void set_value(double value) {
                        this->value += value;
                        if (value > max_value) {
                            this->max_value = value;
                        }
                    }

                    virtual double get_value() {
                        return this->value;
                    }

                    virtual void print() {
                        if (NOT_EMPTY_P(this->name))
                            LOG_INFO("[name=%s][type=%s][value=%f][max value=%f]", this->name->c_str(),
                                     get_type_string().c_str(),
                                     get_value(), max_value);
                    }
                };

                class _avg_metric : public __metric {
                private:
                    uint64_t count = 0;

                public:
                    _avg_metric(string name) : __metric(name) {
                        this->type = AverageMetric;
                    }

                    _avg_metric(string name, bool thread_safe) : __metric(name, thread_safe) {
                        this->type = AverageMetric;
                    }

                    void set_value(double value) override {
                        __metric::set_value(value);
                        this->count++;
                    }

                    double get_value() override {
                        if (this->count > 0) {
                            return (this->value / this->count);
                        }
                        return 0.0;
                    }

                    void print() override {
                        if (NOT_EMPTY_P(this->name)) {
                            LOG_INFO("[name=%s][type=%s][total=%f][average=%f][max value=%f][count=%lu]",
                                     this->name->c_str(),
                                     get_type_string().c_str(), this->value, get_value(),
                                     this->max_value, this->count);
                        }
                    }
                };

                class metrics_utils {
                private:
                    static mutex g_lock;
                    static unordered_map<string, __metric *> *metrics;
                    static __state__ state;

                public:
                    static void init() {
                        std::lock_guard<std::mutex> guard(g_lock);

                        state.set_state(Available);
                    }

                    static bool create_metric(string name, __metric_type_enum type, bool thread_safe) {
                        if (!state.is_available())
                            return false;

                        std::lock_guard<std::mutex> guard(g_lock);

                        if (NOT_NULL(metrics)) {
                            unordered_map<string, __metric *>::iterator iter = metrics->find(name);
                            if (iter != metrics->end()) {
                                return true;
                            }

                            __metric *metric = nullptr;
                            if (type == BasicMetric) {
                                metric = new __metric(name, thread_safe);
                            } else if (type == AverageMetric) {
                                metric = new _avg_metric(name, thread_safe);
                            }

                            if (NOT_NULL(metric)) {
                                metrics->insert(make_pair(*metric->get_name(), metric));
                                return true;
                            }
                        }
                        return false;
                    }

                    static void dispose() {
                        if (!state.is_available())
                            return;

                        std::lock_guard<std::mutex> guard(g_lock);
                        state.set_state(Disposed);

                        if (!IS_EMPTY_P(metrics)) {
                            unordered_map<string, __metric *>::iterator iter;
                            for (iter = metrics->begin(); iter != metrics->end(); iter++) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    delete (metric);
                                }
                            }

                            metrics->clear();

                        }
                    }

                    static bool update(string name, double value) {
                        CHECK_STATE_AVAILABLE(state);

                        if (NOT_NULL(metrics)) {
                            unordered_map<string, __metric *>::iterator iter = metrics->find(name);
                            if (iter != metrics->end()) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    if (metric->is_thread_safe()) {
                                        std::lock_guard<std::mutex> guard(g_lock);
                                        metric->set_value(value);
                                    } else {
                                        metric->set_value(value);
                                    }
                                }
                                return true;
                            }
                        }
                        return false;
                    }

                    static bool timed_metric(string name, timer t) {
                        t.stop();

                        uint64_t elapsed = t.get_elapsed();

                        return update(name, elapsed);
                    }

                    static void dump() {
                        CHECK_STATE_AVAILABLE(state);

                        if (!IS_EMPTY_P(metrics)) {
                            unordered_map<string, __metric *>::iterator iter;
                            for (iter = metrics->begin(); iter != metrics->end(); iter++) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    metric->print();
                                }
                            }
                        }
                    }

                    static void reset_hourly_metrics(__hourly_usage_metric *metric) {
                        metric->reset_time = time_utils::now();
                        metric->current_index = 0;
                        metric->total_time = 0;
                        metric->total_value = 0;
                        for (uint16_t ii = 0; ii < metric->size; ii++) {
                            metric->records[ii].index = ii;
                            metric->records[ii].time = 0;
                            metric->records[ii].value = 0;
                        }
                    }

                    static void update_hourly_metrics(__hourly_usage_metric *metric, uint64_t value, uint64_t time) {
                        uint32_t hours = time_utils::get_hour_diff(metric->reset_time);
                        if (hours > 0) {
                            if (hours >= metric->size) {
                                reset_hourly_metrics(metric);
                            } else {
                                uint16_t max = (hours >= metric->size ? metric->size : hours);
                                uint16_t index = metric->current_index + 1;
                                for (uint16_t ii = 0; ii < max; ii++) {
                                    metric->total_value -= metric->records[index].value;
                                    metric->total_time -= metric->records[index].time;

                                    metric->records[index].index = ii;
                                    metric->records[index].time = 0;
                                    metric->records[index].value = 0;
                                    index++;
                                    if (index >= metric->size) {
                                        index = 0;
                                    }
                                }
                                metric->current_index++;
                            }
                        }
                        metric->total_value += value;
                        metric->total_time += time;
                        metric->records[metric->current_index].value += value;
                        metric->records[metric->current_index].time += time;
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_METRICS_H
