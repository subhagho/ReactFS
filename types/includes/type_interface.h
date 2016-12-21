//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_TYPE_INTERFACE_H
#define REACTFS_TYPE_INTERFACE_H

#include <unordered_map>
#include <vector>

#include "common/includes/common.h"
#include "core/includes/core.h"

REACTFS_NS_CORE
                namespace types {
                    class __typedef {
                    protected:
                        string name;
                    public:
                        virtual ~__typedef() {}

                        virtual void set_datamap(unordered_map<uint8_t, void *> *datamap) = 0;
                    };

                    class __schema : public __typedef {
                    public:
                        virtual void set_keymap(const string &key_name, unordered_map<uint8_t, void *> *keymap) = 0;
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_TYPE_INTERFACE_H
