//
// Created by Subhabrata Ghosh on 04/11/16.
//

#ifndef REACTFS_READ_WRITE_LOCK_H
#define REACTFS_READ_WRITE_LOCK_H

#include "exclusive_lock.h"
#include "timer.h"
#include "__alarm.h"

#define DEFAULT_RW_LOCK_RETRY 10

using namespace com::watergate::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                class read_write_lock {
                private:
                    exclusive_lock *lock;
                    bool write_locked = false;
                    uint16_t readers = 0;

                public:
                    read_write_lock(const string *name) {
                        lock = new exclusive_lock(name);
                    }

                    ~read_write_lock() {
                        CHECK_AND_FREE(lock);
                    }

                    bool write_lock(uint64_t timeout) {
                        CHECK_NOT_NULL(lock);

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (!write_locked && readers == 0) {
                                    write_locked = true;
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
                            alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                        return locked;
                    }

                    bool write_lock() {
                        return write_lock(0);
                    }

                    bool read_lock(uint64_t timeout) {
                        CHECK_NOT_NULL(lock);

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (!write_locked) {
                                    readers++;
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
                            alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                        return locked;
                    }

                    bool read_lock() {
                        return read_lock(0);
                    }

                    bool release_write_lock() {
                        CHECK_NOT_NULL(lock);

                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (write_locked) {
                                    write_locked = false;
                                    locked = true;
                                }
                                lock->release_lock();
                                break;
                            }
                            alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                    }

                    bool release_read_lock() {
                        CHECK_NOT_NULL(lock);

                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (readers > 0) {
                                    readers--;
                                    locked = true;
                                }
                                lock->release_lock();
                                break;
                            }
                            alarm a(DEFAULT_RW_LOCK_RETRY);
                            if (!a.start()) {
                                throw BASE_ERROR("Error starting sleep timer.");
                            }
                        }
                    }
                };
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_H
