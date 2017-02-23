//
// Created by Subhabrata Ghosh on 22/02/17.
//

#ifndef REACTFS_MEMORY_MANAGER_H
#define REACTFS_MEMORY_MANAGER_H

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <mutex>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"
#include "common/includes/buffer_utils.h"

REACTFS_NS_COMMON
                namespace mm {
                    class memory_block {
                    private:
                        uint32_t used_flag = 0;
                        uint32_t size = 0;
                        void *ptr = nullptr;
                        void *data_ptr = nullptr;
                        uint32_t *used = nullptr;
                        memory_block *next = nullptr;
                        void *overflow_ptr = nullptr;

                    public:
                        memory_block(const uint32_t size, void *ptr) {
                            PRECONDITION(size > 0);
                            CHECK_NOT_NULL(ptr);
                            PRECONDITION(used_flag > 0);

                            srand(time(NULL));
                            this->used_flag = rand();

                            this->size = sizeof(uint32_t) + size;
                            memset(ptr, 0, this->size);

                            used = static_cast<uint32_t *>(ptr);
                            this->ptr = ptr;
                            this->data_ptr = buffer_utils::increment_data_ptr(ptr, sizeof(uint32_t));
                        }

                        ~memory_block() {
                            PRECONDITION(!is_reserved());

                            this->ptr = nullptr;
                            this->data_ptr = nullptr;
                            this->used = nullptr;
                        }

                        bool reserve() {
                            if (*used == 0) {
                                *used = used_flag;
                                overflow_ptr = nullptr;
                                return true;
                            }
                            return false;
                        }

                        bool is_reserved() {
                            return (*used == used_flag);
                        }

                        void release(bool clear = false) {
                            if (*used == used_flag) {
                                *used = 0;
                                overflow_ptr = nullptr;
                                if (clear) {
                                    memset(this->ptr, 0, this->size);
                                }
                            }
                        }

                        void *get() {
                            PRECONDITION(*used == used_flag);
                            return data_ptr;
                        }

                        void set_next(memory_block *next) {
                            this->next = next;
                        }

                        memory_block *get_next() {
                            return this->next;
                        }

                        bool has_next() {
                            return NOT_NULL(this->next);
                        }
                    };

                    class mblock_pool {
                    private:
                        void *base_ptr = nullptr;
                        memory_block *head = nullptr;
                        memory_block *used_blocks = nullptr;
                        memory_block *free_blocks = nullptr;
                        uint32_t block_size = 0;
                        uint32_t count = 0;

                        void remove_free_block(memory_block *prev, memory_block *b) {
                            CHECK_NOT_NULL(free_blocks);
                            CHECK_NOT_NULL(prev);
                            CHECK_NOT_NULL(b);
                            if (prev == free_blocks) {
                                POSTCONDITION(b == free_blocks->get_next());
                                free_blocks = free_blocks->get_next();
                            } else {
                                POSTCONDITION(prev->get_next() == b);
                                prev->set_next(b->get_next());
                            }
                        }

                        void add_free_block(memory_block *b) {
                            CHECK_NOT_NULL(b);
                            if (IS_NULL(free_blocks)) {
                                b->set_next(nullptr);
                                free_blocks = b;
                            } else {
                                memory_block *next = free_blocks->get_next();
                                b->set_next(next);
                                free_blocks = b;
                            }
                        }

                        void remove_used_block(memory_block *prev, memory_block *b) {
                            CHECK_NOT_NULL(used_blocks);
                            CHECK_NOT_NULL(prev);
                            CHECK_NOT_NULL(b);
                            if (prev == used_blocks) {
                                POSTCONDITION(b == used_blocks->get_next());
                                used_blocks = used_blocks->get_next();
                            } else {
                                POSTCONDITION(prev->get_next() == b);
                                prev->set_next(b->get_next());
                            }
                        }

                        void add_used_block(memory_block *b) {
                            CHECK_NOT_NULL(b);
                            if (IS_NULL(used_blocks)) {
                                b->set_next(nullptr);
                                used_blocks = b;
                            } else {
                                memory_block *next = used_blocks->get_next();
                                b->set_next(next);
                                used_blocks = b;
                            }
                        }

                    public:
                        mblock_pool(uint32_t block_size, uint32_t count) {
                            PRECONDITION(block_size > 0);
                            PRECONDITION(count > 0);

                            uint32_t size = (block_size + sizeof(uint32_t)) * count;
                            base_ptr = malloc(sizeof(BYTE) * size);
                            CHECK_ALLOC(base_ptr, TYPE_NAME(void * ));

                            void *ptr = base_ptr;
                            memory_block *mb = nullptr;
                            memory_block *last = nullptr;
                            for (uint32_t ii = 0; ii < count; ii++) {
                                mb = new memory_block(block_size, ptr);
                                CHECK_ALLOC(mb, TYPE_NAME(memory_block));
                                if (IS_NULL(head)) {
                                    head = mb;
                                } else {
                                    CHECK_NOT_NULL(last);
                                    last->set_next(mb);
                                }
                                last = mb;
                                ptr = buffer_utils::increment_data_ptr(ptr, (block_size + sizeof(uint32_t)));
                            }
                            free_blocks = head;
                        }

                        ~mblock_pool() {
                            PRECONDITION(IS_NULL(free_blocks));
                            memory_block *mb = head;
                            while (NOT_NULL(mb)) {
                                memory_block *next = mb->get_next();
                                CHECK_AND_FREE(mb);
                                mb = next;
                            }
                            FREE_PTR(base_ptr);
                        }

                        bool has_free_block() {
                            return NOT_NULL(free_blocks);
                        }

                        bool is_pool_used() {
                            return NOT_NULL(used_blocks);
                        }

                        memory_block *get_free_block() {
                            CHECK_NOT_NULL(head);
                            if (NOT_NULL(free_blocks)) {
                                memory_block *block = free_blocks;
                                remove_free_block(free_blocks, block);
                                add_used_block(block);

                                return block;
                            }
                            return nullptr;
                        }

                        void *release_block(memory_block *block) {
                            CHECK_NOT_NULL(head);
                            CHECK_NOT_NULL(block);

                            remove_used_block(used_blocks, block);
                            add_free_block(block);
                        }
                    };

                    class memory_array {
                    private:
                        mutex __lock;
                        uint32_t block_size = 0;
                        uint32_t max_array_size = 0;
                        uint32_t min_array_size = 0;
                        uint32_t current_array_size = 0;
                        vector<mblock_pool *> blocks;

                    public:
                        memory_array(const uint32_t block_size, const uint32_t min_size, const uint32_t max_size) {
                            LOCK_GUARD(__lock);
                            PRECONDITION(block_size > 0);
                            PRECONDITION(min_size > 0);
                            PRECONDITION(max_size > min_size);

                            mblock_pool *mp = new mblock_pool(block_size, min_size);
                            CHECK_ALLOC(mp, TYPE_NAME(mblock_pool));

                            blocks.push_back(mp);

                            this->block_size = block_size;
                            this->min_array_size = min_size;
                            this->max_array_size = max_size;
                            this->current_array_size = 1;
                        }

                        ~memory_array() {
                            LOCK_GUARD(__lock);
                            for (mblock_pool *mp : blocks) {
                                CHECK_AND_FREE(mp);
                            }
                            blocks.clear();
                        }

                        void resize() {
                            if (current_array_size == min_array_size) {
                                return;
                            }
                            uint32_t size = current_array_size;
                            while (size > min_array_size) {
                                vector<mblock_pool *>::iterator iter;
                                for (iter = blocks.begin(); iter != blocks.end(); iter++) {
                                    mblock_pool *mp = *iter;
                                    LOCK_GUARD(__lock);
                                    if (!mp->is_pool_used()) {
                                        blocks.erase(iter);
                                        CHECK_AND_FREE(mp);
                                        size--;
                                    }
                                }
                            }
                        }

                        uint32_t get_min_size() {
                            return this->min_array_size;
                        }

                        uint32_t get_max_size() {
                            return this->max_array_size;
                        }

                        uint32_t get_current_size() {
                            return this->current_array_size;
                        }
                    };

                    class memory_manager {

                    };
                }
REACTFS_NS_COMMON_END
#endif //REACTFS_MEMORY_MANAGER_H
