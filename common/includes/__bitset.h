//
// Created by Subhabrata Ghosh on 17/11/16.
//

#ifndef REACTFS_BITSET_H
#define REACTFS_BITSET_H

#include "common.h"
#include "common_utils.h"
#include "base_error.h"

#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                class __bitset {
                private:
                    uint16_t _size;
                    uint64_t *_data = nullptr;
                    bool mapped = false;

                public:
                    __bitset(uint16_t size) {
                        this->_size = size;
                        this->_data = (uint64_t *) malloc(this->_size);
                        CHECK_NOT_NULL(this->_data);
                        memset(this->_set, 0, this->_size);
                        mapped = false;
                    }

                    __bitset(uint16_t size, uint64_t *ptr) {
                        CHECK_NOT_NULL(ptr);
                        this->_size = size;
                        this->_set = ptr;
                        memset(this->_set, 0, this->_size);
                        mapped = true;
                    }

                    ~__bitset() {
                        if (!mapped) {
                            FREE_PTR(this->_data);
                        }
                        this->_data = nullptr;
                    }

                    bool check_and_set(uint32_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        if (!check(offset)) {
                            set(offset);
                            return true;
                        }
                        return false;
                    }

                    bool check(uint32_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t index = (offset / 64);
                        uint16_t part = (offset % 64);
                        PRECONDITION(index < _size);
                        uint64_t set = _data[index];

                        if (CHECK_BIT(set, part) == 0) {
                            return false;
                        } else {
                            return true;
                        }
                    }

                    void set(uint32_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t index = (offset / 64);
                        uint16_t part = (offset % 64);
                        PRECONDITION(index < _size);
                        uint64_t set = _data[index];

                        set != 1 << part;
                    }

                    void clear(uint32_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t index = (offset / 64);
                        uint16_t part = (offset % 64);
                        PRECONDITION(index < _size);
                        uint64_t set = _data[index];

                        set &= ~(1 << part);
                    }

                    int get_free_bit() {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t index = 0;
                        while (index < this->_size) {
                            if (check_and_set(index)) {
                                return index;
                            }
                            index++;
                        }
                        return -1;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_BITSET_H
