//
// Created by Subhabrata Ghosh on 04/11/16.
//

#ifndef REACTFS_READ_WRITE_LOCK_H
#define REACTFS_READ_WRITE_LOCK_H

#include "exclusive_lock.h"
#include "timer.h"
#include "__alarm.h"
#include "process_utils.h"
#include "unordered_map"

#define DEFAULT_RW_LOCK_RETRY 10

using namespace com::watergate::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                typedef struct __owner__ {
                    char owner[SIZE_USER_NAME];
                    char txn_id[SIZE_UUID];
                    pid_t process_id;
                    uint64_t lock_timestamp = 0;
                } __owner;

                typedef struct __lock_struct__ {
                        
                } __lock_struct;

                class read_write_lock {
                private:
                    exclusive_lock *lock;
                    bool write_locked = false;
                    uint16_t readers = 0;
                    __owner owner;
                    unordered_map<string, uint64_t> reader_threads;

                    bool has_thread_lock(string thread_id) {
                        unordered_map<std::string, uint64_t>::const_iterator iter = reader_threads.find(thread_id);
                        if (iter != reader_threads.end()) {
                            return true;
                        }
                        return false;
                    }

                public:
                    read_write_lock(const string *name) {
                        lock = new exclusive_lock(name);
                    }

                    ~read_write_lock() {
                        CHECK_AND_FREE(lock);
                    }

                    string write_lock(string owner, uint64_t timeout) {
                        CHECK_NOT_NULL(lock);
                        PRECONDITION(!IS_EMPTY(owner));

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        string txn_id;
                        while (true) {
                            if (lock->try_lock()) {
                                if (!write_locked && readers == 0) {
                                    write_locked = true;

                                    this->owner.lock_timestamp = time_utils::now();
                                    memset(this->owner.owner, SIZE_USER_NAME, 0);
                                    strncpy(this->owner.owner, owner.c_str(), owner.length());
                                    this->owner.process_id = getpid();
                                    memset(this->owner.txn_id, 0, SIZE_UUID);
                                    txn_id = common_utils::uuid();
                                    strncpy(this->owner.txn_id, txn_id, txn_id.length());

                                    locked = true;
                                }
                                lock->release_lock();
                            }
                            if (locked) {
                                break;
                            }
                            uint64_t now = time_utils::now();
                            if (startt - now > timeout) {
                                break;
                            }
                            __alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                        return txn_id;
                    }

                    string write_lock(string owner) {
                        return write_lock(owner, 0);
                    }

                    bool read_lock(uint64_t timeout) {
                        CHECK_NOT_NULL(lock);

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (!write_locked) {
                                    string thread_id = thread_utils::get_current_thread();
                                    if (!has_thread_lock(thread_id)) {
                                        reader_threads[thread_id] = time_utils::now();
                                        readers++;
                                    }
                                    locked = true;
                                }
                                lock->release_lock();
                            }
                            if (locked) {
                                break;
                            }
                            uint64_t now = time_utils::now();
                            if (startt - now > timeout) {
                                break;
                            }
                            __alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                        return locked;
                    }

                    bool read_lock() {
                        return read_lock(0);
                    }

                    bool release_write_lock(string txn_id) {
                        CHECK_NOT_NULL(lock);
                        PRECONDITION(!IS_EMPTY(txn_id));

                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (write_locked) {
                                    pid_t pid = getpid();
                                    if (this->owner.process_id == pid &&
                                        strncmp(this->owner.txn_id, txn_id.c_str(), txn_id.length()) == 0) {
                                        this->owner.process_id = -1;
                                        this->owner.lock_timestamp = 0;
                                        memset(this->owner.txn_id, 0, SIZE_UUID);
                                        memset(this->owner.owner, 0, SIZE_USER_NAME);

                                        write_locked = false;
                                        locked = true;
                                    }
                                }
                                lock->release_lock();
                                break;
                            }
                            __alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                        return locked;
                    }

                    bool release_read_lock() {
                        CHECK_NOT_NULL(lock);

                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (readers > 0) {
                                    string thread_id = thread_utils::get_current_thread();
                                    if (has_thread_lock(thread_id)) {
                                        reader_threads.erase(thread_id);
                                        readers--;
                                        locked = true;
                                    }
                                }
                                lock->release_lock();
                                break;
                            }
                            __alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                        return locked;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_H
