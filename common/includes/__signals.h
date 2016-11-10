//
// Created by Subhabrata Ghosh on 10/11/16.
//

#ifndef REACTFS_SIGNALS_H
#define REACTFS_SIGNALS_H

#include <unordered_map>
#include <mutex>

#include "common.h"
#include "base_error.h"
#include "log_utils.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Struct to define a chain of callback(s) to be invoked in sequence.
                 */
                typedef struct __callback_chain__ {
                    /// Callback to be invoked.
                    __callback *callback = nullptr;
                    /// Next chain pointer.
                    __callback_chain__ *next = nullptr;
                } __callback_chain;

                /*!
                 * Custom singleton to define and register signal handlers to be invoked with the correct context.
                 * This is to overcome the default signal handling framework where the handler method doesn't have any
                 * context to react to OS signals.
                 */
                class __signals {
                private:
                    /// Thread mutext lock
                    static mutex _lock;
                    /// Map to register handler chains for each type of signal
                    static unordered_map<int, __callback_chain *> signal_map;

                    /*!
                     * Handler function to be registered and invoked with the OS.
                     *
                     * @param signal - Signal constant
                     */
                    static void signal_handler(int signal) {
                        std::lock_guard<std::mutex> guard(_lock);

                        LOG_DEBUG("Signal handler invoked with signal [%d]", signal);

                        unordered_map<int, __callback_chain *>::iterator iter = signal_map.find(signal);
                        if (iter != signal_map.end()) {
                            __callback_chain *ptr = iter->second;
                            while (NOT_NULL(ptr)) {
                                if (NOT_NULL(ptr->callback)) {
                                    LOG_DEBUG("Invoking signal handler delegate. [uuid=%s]",
                                              ptr->callback->get_uuid().c_str());
                                    ptr->callback->set_state(__callback_state_enum::ERROR);
                                    ptr->callback->error();
                                }
                                ptr = ptr->next;
                            }
                        }
                    }

                public:
                    /*!
                     * Register a new signal handler for the specified signal event.
                     *
                     * @param signal - Signal constant
                     * @param c - Handler callback.
                     */
                    static void add_handler(int signal, __callback *c) {
                        PRECONDITION(NOT_NULL(c));

                        std::lock_guard<std::mutex> guard(_lock);

                        unordered_map<int, __callback_chain *>::iterator iter = signal_map.find(signal);
                        if (iter != signal_map.end()) {
                            __callback_chain *ptr = iter->second;
                            if (NOT_NULL(ptr)) {
                                while (NOT_NULL(ptr) && NOT_NULL(ptr->next)) {
                                    // If the same callback instance is already registered
                                    // ingore this call.
                                    if (ptr->callback == c) {
                                        return;
                                    }
                                    ptr = ptr->next;
                                }
                                __callback_chain *next = new __callback_chain();
                                next->callback = c;
                                next->next = nullptr;
                                ptr->next = next;
                            } else {
                                __callback_chain *next = new __callback_chain();
                                next->callback = c;
                                next->next = nullptr;
                                signal_map[signal] = next;
                            }
                        } else {
                            signal(signal, &signal_handler);

                            __callback_chain *next = new __callback_chain();
                            next->callback = c;
                            next->next = nullptr;

                            signal_map[signal] = next;
                        }
                    }

                    /*!
                     * Remove a registered instance of a signal handler callback.
                     *
                     * @param signal - Signal constant
                     * @param uuid - UUID of the registered callback instance.
                     */
                    static void remove_handler(int signal, string uuid) {
                        CHECK_NOT_EMPTY(uuid);

                        std::lock_guard<std::mutex> guard(_lock);

                        unordered_map<int, __callback_chain *>::iterator iter = signal_map.find(signal);
                        if (iter != signal_map.end()) {
                            __callback_chain *ptr = iter->second;
                            while (NOT_NULL(ptr)) {
                                if (ptr->callback->get_uuid() == uuid) {
                                    if (ptr == iter->second) {
                                        if (IS_NULL(ptr->next)) {
                                            signal_map[signal] = nullptr;
                                        } else {
                                            signal_map[signal] = ptr->next;
                                        }
                                        break;
                                    }
                                } else if (NOT_NULL(ptr->next)) {
                                    if (NOT_NULL(ptr->next->callback)) {
                                        if (ptr->next->callback->get_uuid() == uuid) {
                                            __callback_chain *nptr = ptr->next->next;
                                            __callback_chain *rptr = ptr->next;
                                            ptr->next = nptr;

                                            CHECK_AND_FREE(rptr);

                                            break;
                                        }
                                    }
                                }
                                ptr = ptr->next;
                            }
                        }
                    }

                    /*!
                     * Remove a registered instance of a signal handler callback.
                     *
                     * @param signal - Signal constant
                     * @param c - Registered callback pointer.
                     */
                    static void remove_handler(int signal, __callback *c) {
                        CHECK_NOT_NULL(c);
                        remove_handler(c->get_uuid());
                    }

                    /*!
                    * Remove a registered instance of a signal handler callback.
                    *
                    * @param signal - Signal constant
                    * @param c - Registered callback.
                    */
                    static void remove_handler(int signal, __callback c) {
                        remove_handler(c.get_uuid());
                    }

                    /*!
                     * Dispose the signal handler singleton. This does not free the callback instances.
                     */
                    static void dispose() {
                        std::lock_guard<std::mutex> guard(_lock);
                        if (!singal_map.empty()) {
                            unordered_map<int, __callback_chain *>::iterator iter = signal_map.begin();
                            while (iter != signal_map.end()) {
                                __callback_chain *ptr = iter->second;
                                while (NOT_NULL(ptr)) {
                                    __callback_chain *next = ptr->next;
                                    delete (ptr);
                                    ptr = next;
                                }
                                iter++;
                            }
                            signal_map.clear();
                        }
                    }
                };
            }
        }
    }
}
#endif //REACTFS_SIGNALS_H
