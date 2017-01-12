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

#include <atomic>
#include <mutex>
#include <unordered_map>

#include "common.h"
#include "common_utils.h"
#include "timer.h"
#include "log_utils.h"
#include "__state__.h"

#define START_TIMER(n, i)  \
    timer _t_##i; \
    _t_##i.start();

#define END_TIMER(m, i) do { \
    if (!IS_EMPTY(m)) \
        com::wookler::reactfs::common::metrics_utils::timed_metric(m, _t_##i); \
} while(0);

#define DUMP_METRIC(n) do { \
    if (!IS_EMPTY(n)) \
        com::wookler::reactfs::common::metrics_utils::dump(n); \
} while(0);

#define REMOVE_METRIC(n) do { \
    if (!IS_EMPTY(n)) \
        com::wookler::reactfs::common::metrics_utils::remove(n); \
} while(0);

REACTFS_NS_COMMON

                /*!
                 * Enum to define the types of metrics.
                 */
                enum __metric_type_enum {
                    BasicMetric, AverageMetric
                };

                /*!
                 * Base metrics class, captures a simple value metric.
                 */
                class __metric {
                protected:
                    /// Name of this metric
                    string *name = nullptr;

                    /// Type of this metric.
                    __metric_type_enum type;

                    /// Metric value
                    atomic<uint64_t> value;

                    /// Max value this metric can have.
                    atomic<uint64_t> max_value;

                public:
                    /*!<constructor
                     * Default constructor.
                     *
                     * @param name - Metric name.
                     */
                    __metric(string name) {
                        PRECONDITION(!IS_EMPTY(name));

                        this->type = BasicMetric;
                        this->name = new string(name);
                        CHECK_ALLOC(this->name, TYPE_NAME(string));
                        this->value.store(0);
                        this->max_value.store(0);
                    }

                    /*!<destructor
                     * Default desctructor.
                     */
                    virtual ~__metric() {
                        CHECK_AND_FREE(name);
                    }

                    /*!
                     * Get the name of this metric.
                     *
                     * @return - Metric name
                     */
                    string *get_name() {
                        return this->name;
                    }

                    /*!
                     * Get the type of this metric.
                     *
                     * @return - Metric type.
                     */
                    __metric_type_enum get_type() {
                        return this->type;
                    }

                    /*!
                     * Get the metric type as string.
                     *
                     * @return - Metric type string.
                     */
                    string get_type_string() {
                        switch (type) {
                            case BasicMetric:
                                return "BasicMetric";
                            case AverageMetric:
                                return "AverageMetric";
                        }
                        return common_consts::EMPTY_STRING;
                    }

                    /*!
                     * Overwrite the value of this metric.
                     *
                     * @param value - Value to overwrite with.
                     */
                    virtual uint64_t set_value(uint64_t value) {
                        this->value = value;
                        return this->value;
                    }

                    /*!
                     * Get the value of this metric.
                     *
                     * @return - Metric value.
                     */
                    virtual double get_value() {
                        return this->value;
                    }

                    /*!
                     * Increment this metric value.
                     *
                     * @param value - Value to increment by.
                     */
                    virtual uint64_t increment(uint64_t value) {
                        this->value += value;
                        uint64_t m_value = this->max_value;
                        if (value > m_value) {
                            while (!this->max_value.compare_exchange_weak(m_value, value, std::memory_order_release,
                                                                          std::memory_order_relaxed)) {
                                m_value = this->max_value;
                                if (value <= m_value) {
                                    break;
                                }
                            }
                        }
                        return this->value;
                    }

                    virtual double get_max_value() {
                        return this->max_value;
                    }

                    /*!
                     * Print this metric to the log.
                     */
                    virtual void print() {
                        if (NOT_EMPTY_P(this->name))
                            LOG_INFO("[name=%s][type=%s][value=%lu][max value=%lu]", this->name->c_str(),
                                     get_type_string().c_str(),
                                     get_value(), get_max_value());
                    }

                };

                /*!
                 * Metric class capturing averages.
                 */
                class __avg_metric : public __metric {
                private:
                    atomic<uint64_t> count;

                public:
                    /*!<constructor
                     * Default constructor.
                     *
                     * @param name - Metric name.
                     */
                    __avg_metric(string name) : __metric(name) {
                        this->type = AverageMetric;
                        this->count.store(0);
                    }

                    /*!
                     * Increment this metric value, will increment the denomintor count by 1.
                     *
                     * @param value - Value to increment this metric by.
                     * @return - Incremented average value.
                     */
                    uint64_t increment(uint64_t value) override {
                        __metric::increment(value);
                        this->count++;

                        return get_value();
                    }

                    /*!
                     * Get the value average for this metric.
                     *
                     * @return - Average value.
                     */
                    double get_value() override {
                        if (this->count > 0) {
                            return (((double) this->value) / this->count);
                        }
                        return 0;
                    }

                    /*!
                     * Overwrite this metric value with the specified values.
                     *
                     * @param value - Value to overwrite.
                     * @param div - Denominator value to overwrite.
                     * @return - New average value.
                     */
                    uint64_t set(uint64_t value, uint64_t div) {
                        __metric::set_value(value);
                        this->count = div;

                        return get_value();
                    }

                    /*!
                     * Increment the metric value with the specified value and denominator.
                     *
                     * @param value - Value to increment by.
                     * @param div - Denominator value increment.
                     * @return - New average value.
                     */
                    uint64_t increment(uint64_t value, uint64_t div) {
                        __metric::increment(value);
                        this->count += div;
                        if (div > 0) {
                            uint64_t a = (value * 100) / div;
                            uint64_t m_value = max_value;
                            if (a > m_value) {
                                while (!this->max_value.compare_exchange_weak(m_value, a, std::memory_order_release,
                                                                              std::memory_order_relaxed)) {
                                    m_value = this->max_value;
                                    if (a <= m_value) {
                                        break;
                                    }
                                }
                            }
                        }
                        return get_value();
                    }

                    double get_max_value() override {
                        return ((double) this->max_value) / 100;
                    }

                    uint64_t get_count() {
                        return this->count;
                    }

                    /*!
                     * Print this metric to the log.
                     */
                    void print() override {
                        if (NOT_EMPTY_P(this->name)) {
                            uint64_t v = this->value;
                            uint64_t c = this->count;
                            LOG_INFO("[name=%s][type=%s][total=%lu][average=%f][max value=%f][count=%lu]",
                                     this->name->c_str(),
                                     get_type_string().c_str(), v, get_value(),
                                     get_max_value(), c);
                        }
                    }
                };

                typedef unordered_map<string, __metric *> __metrics_map;

                /*!
                 * Utility class to define and manage global metrics.
                 */
                class metrics_utils {
                private:
                    /// Lock to control concurrent updates.
                    static mutex g_lock;

                    /// Map containing all the defined global metrics.
                    static __metrics_map *metrics;

                    /// State of this metrics utility.
                    static __state__ state;

                public:
                    /*!
                     * Initialize the metrics utility.
                     */
                    static void init() {
                        std::lock_guard<std::mutex> guard(g_lock);
                        metrics = new __metrics_map();
                        CHECK_ALLOC(metrics, TYPE_NAME(unordered_map));
                        state.set_state(Available);
                    }

                    /*!
                     * Create a new metric with the specified parameters.
                     *
                     * @param name
                     * @param type
                     * @return
                     */
                    static bool create_metric(const string &name, __metric_type_enum type) {
                        if (!state.is_available())
                            return false;

                        std::lock_guard<std::mutex> guard(g_lock);

                        if (NOT_NULL(metrics)) {
                            __metrics_map::iterator iter = metrics->find(name);
                            if (iter != metrics->end()) {
                                return true;
                            }

                            __metric *metric = nullptr;
                            if (type == BasicMetric) {
                                metric = new __metric(name);
                                CHECK_ALLOC(metric, TYPE_NAME(__metric));
                            } else if (type == AverageMetric) {
                                metric = new __avg_metric(name);
                                CHECK_ALLOC(metric, TYPE_NAME(_avg_metric));
                            }

                            if (NOT_NULL(metric)) {
                                metrics->insert(make_pair(*(metric->get_name()), metric));
                                return true;
                            }
                        }
                        return false;
                    }

                    static void remove(const string &name) {
                        if (!state.is_available())
                            return;

                        std::lock_guard<std::mutex> guard(g_lock);

                        if (NOT_NULL(metrics)) {
                            metrics->erase(name);
                        }
                    }

                    static void dispose() {
                        std::lock_guard<std::mutex> guard(g_lock);
                        state.set_state(Disposed);

                        if (!IS_EMPTY_P(metrics)) {
                            __metrics_map::iterator iter;
                            for (iter = metrics->begin(); iter != metrics->end(); iter++) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    delete (metric);
                                }
                            }
                            metrics->clear();
                        }
                        CHECK_AND_FREE(metrics);
                    }

                    static bool update(const string &name, uint64_t value) {
                        if (!state.is_available())
                            return false;

                        if (NOT_NULL(metrics)) {
                            __metrics_map::iterator iter = metrics->find(name);
                            if (iter != metrics->end()) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    metric->increment(value);
                                }
                                return true;
                            }
                        }
                        return false;
                    }

                    static bool update(const string &name, uint64_t value, uint64_t div) {
                        if (!state.is_available())
                            return false;

                        if (NOT_NULL(metrics)) {
                            __metrics_map::iterator iter = metrics->find(name);
                            if (iter != metrics->end()) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric) && metric->get_type() == __metric_type_enum::AverageMetric) {
                                    __avg_metric *avg = dynamic_cast<__avg_metric *>(metric);
                                    CHECK_CAST(avg, TYPE_NAME(__metric), TYPE_NAME(__avg_metric));
                                    avg->increment(value, div);
                                }
                                return true;
                            }
                        }
                        return false;
                    }

                    static bool timed_metric(const string &name, timer t) {
                        t.stop();
                        uint64_t elapsed = t.get_elapsed();

                        return update(name, elapsed);
                    }

                    static void dump() {
                        if (!state.is_available())
                            return;

                        if (!IS_EMPTY_P(metrics)) {
                            __metrics_map::iterator iter;
                            for (iter = metrics->begin(); iter != metrics->end(); iter++) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    metric->print();
                                }
                            }
                        }
                    }

                    static void dump(string name) {
                        if (!state.is_available())
                            return;

                        if (!IS_EMPTY_P(metrics)) {
                            __metrics_map::iterator iter = metrics->find(name);
                            if (iter != metrics->end()) {
                                __metric *metric = iter->second;
                                if (NOT_NULL(metric)) {
                                    metric->print();
                                }
                            }
                        }
                    }
                };
REACTFS_NS_COMMON_END
#endif //WATERGATE_METRICS_H
