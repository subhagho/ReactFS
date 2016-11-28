//
// Created by Subhabrata Ghosh on 17/11/16.
//

#ifndef REACTFS_BITSET_H
#define REACTFS_BITSET_H

#include "common.h"
#include "common_utils.h"
#include "base_error.h"

#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))
#define BITSET_INDEX(o) (o / 32)
#define BIT_INDEX(o) (o % 32)
#define ARRAY_SIZE(s) (s / sizeof(uint32_t))

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Expandable bitset implementation for use with memory mapped storage. Can also be used
                 * as a standalone bitset similar to std::bitset.
                 *
                 * This implementation is not thread safe hence concurrency needs to be managed by the callers.
                 * The reason for not managing concurrency is that in case of memory mapped storage, thread locking
                 * will not be sufficient to manage concurrency.
                 *
                 */
                class __bitset {
                private:
                    /// Number of available bits.
                    uint32_t _size = 0;
                    /// Bitset storage pointer. (assumed to be contigeous uint64_t pointer).
                    uint32_t *_data = nullptr;
                    /// Is the storage mapped to an external memory segment.
                    bool mapped = false;
                    /// Max bitmap index offset
                    uint16_t max_offset = 0;
                public:
                    /*!< constructor
                     *
                     * Default constructor which will allocate memory for the bitset storage.
                     *
                     * @param count - Number of bits in the bitset.
                     * @return
                     */
                    __bitset(uint16_t count) {
                        PRECONDITION(count > 0);
                        this->max_offset = count;
                        this->_size = get_byte_size(count);

                        this->_data = (uint32_t *) malloc(this->_size);
                        CHECK_ALLOC(this->_data, TYPE_NAME(uint32_t *));

                        mapped = false;
                    }

                    /*!< constructor
                     *
                     * Constructor which uses an external memory segment for storage
                     * (to be used for memory mapped storage)
                     *
                     * @param ptr - External memory pointer.
                     * @param size - Number of bits in the bitset.
                     * @return
                     */
                    __bitset(uint32_t *ptr, uint32_t size) {
                        CHECK_NOT_NULL(ptr);
                        PRECONDITION(size > 0);

                        this->_size = size;
                        this->max_offset = get_max_offset(size);

                        this->_data = ptr;

                        mapped = true;
                    }

                    /*! < destructor
                     *
                     * Free the storage if locally allocated.
                     */
                    ~__bitset() {
                        if (!mapped) {
                            FREE_PTR(this->_data);
                        }
                        this->_data = nullptr;
                    }

                    /*!
                     * Get the number of bits supported by this set.
                     *
                     * @return - # of bits
                     */
                    uint16_t get_size() {
                        return this->max_offset;
                    }

                    /*!
                     * Get the storage byte size of this bitset.
                     *
                     * @return - Storage byte size.
                     */
                    uint32_t get_byte_size() {
                        return this->_size;
                    }

                    /*!
                     * Check and set the specified bit.
                     *
                     * @param offset - Bit to set.
                     * @return - True if set, else false if bit is already set.
                     */
                    bool check_and_set(uint32_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        if (!check(offset)) {
                            set(offset);
                            return true;
                        }
                        return false;
                    }

                    /*!
                     * Check if the specified bit is set.
                     *
                     * @param offset - Bit to check.
                     * @return - Is bit set?
                     */
                    bool check(uint16_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        PRECONDITION(offset < this->max_offset);

                        uint16_t index = BITSET_INDEX(offset);
                        uint16_t part = BIT_INDEX(offset);

                        if (CHECK_BIT(_data[index], part) == 0) {
                            return false;
                        } else {
                            return true;
                        }
                    }

                    /*!
                     * Set the specified bit to true.
                     * Method doesn't check if the bit is already set.
                     *
                     * @param offset - Bit to set
                     */
                    void set(uint16_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        PRECONDITION(offset < this->max_offset);

                        uint16_t index = BITSET_INDEX(offset);
                        uint16_t part = BIT_INDEX(offset);

                        _data[index] |= (1 << part);
                    }

                    /*!
                     * Clear the specified bit.
                     *
                     * @param offset - Bit to clear.
                     */
                    void clear(uint16_t offset) {
                        CHECK_NOT_NULL(this->_data);
                        PRECONDITION(offset < this->max_offset);

                        uint16_t index = BITSET_INDEX(offset);
                        uint16_t part = BIT_INDEX(offset);

                        _data[index] &= ~(1 << part);
                    }

                    /*!
                     * Check if any bit in this bitset is true.
                     *
                     * @return - Any bit set?
                     */
                    bool any() {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t array_s = ARRAY_SIZE(this->_size);
                        for (uint16_t ii = 0; ii < array_s; ii++) {
                            if (_data[ii] > 0) {
                                return true;
                            }
                        }
                        return false;
                    }

                    /*!
                     * Check to see if none of the bits in this bitset is true.
                     *
                     * @return - All bits are off?
                     */
                    bool none() {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t array_s = ARRAY_SIZE(this->_size);
                        for (uint16_t ii = 0; ii < array_s; ii++) {
                            if (_data[ii] > 0) {
                                return false;
                            }
                        }
                        return true;
                    }

                    /*!
                     * Check to see if all the bits in this bitset are true.
                     *
                     * @return - All bits are set?
                     */
                    bool all() {
                        CHECK_NOT_NULL(this->_data);
                        uint16_t array_s = ARRAY_SIZE(this->_size);
                        for (uint16_t ii = 0; ii < array_s; ii++) {
                            if (_data[ii] == 0) {
                                return false;
                            }
                        }
                        return true;
                    }

                    /*!
                     * Get the first free bit and set it. Will return the index of the bit set.
                     *
                     * @return - Index of the bit set, or -1 if all bits are already set.
                     */
                    int get_free_bit() {
                        CHECK_NOT_NULL(this->_data);
                        uint64_t m_value = UINT_MAX;
                        uint16_t index = 0;
                        uint16_t array_s = ARRAY_SIZE(this->_size);
                        for (uint16_t ii = 0; ii < array_s; ii++) {
                            if (_data[ii] < m_value) {
                                break;
                            }
                            index += 32;
                        }
                        while (index < this->max_offset) {
                            if (check_and_set(index)) {
                                return index;
                            }
                            index++;
                        }
                        return -1;
                    }

                    /*!
                     * Clear all the bits in this bitset.
                     *
                     */
                    void clear() {
                        memset(this->_data, 0, this->_size);
                    }

                    /*!
                     * Static method to get the storage size required for the specfied number of bits.
                     *
                     * @param count - Number of bits.
                     * @return - Storage size in bytes.
                     */
                    static uint32_t get_byte_size(uint16_t count) {
                        if (count > 0) {
                            uint16_t s = count / 32;
                            if (count % 32 > 0) {
                                s += 1;
                            }
                            return (s * sizeof(uint32_t));
                        }
                        return 0;
                    }

                    /*!
                     * Get the max bit offset supported based on the specified data size.
                     *
                     * @param size - Data size in bytes
                     * @return - Max bit offset supported
                     */
                    static uint16_t get_max_offset(uint32_t size) {
                        if (size > 0) {
                            uint32_t c = size / sizeof(uint32_t);
                            return c * 32;
                        }
                        return 0;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_BITSET_H
