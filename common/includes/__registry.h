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
// Created by Subhabrata Ghosh on 18/11/16.
//

#ifndef REACTFS_REGISTRY_H
#define REACTFS_REGISTRY_H

#include <unordered_map>
#include <mutex>

#include "common.h"
#include "common_utils.h"
#include "base_error.h"
#include "config.h"

using namespace std;

REACTFS_NS_COMMON
                template<class _Key, class _Type>
                class __registry {
                private:
                    mutex __lock;
                    unordered_map<_Key, _Type *> registry;
                    const ConfigValue *config = nullptr;

                public:
                    ~__registry() {
                        clear();
                        config = nullptr;
                    }

                    void set_config(const ConfigValue *config) {
                        CHECK_NOT_NULL(config);
                        this->config = config;
                    }

                    void add(_Key key, _Type *type, bool configure = false);

                    bool contains(_Key key);

                    _Type *get(_Key key);

                    _Type *remove(_Key key);

                    void clear();
                };
REACTFS_NS_COMMON_END

template<class _Key, class _Type>
void com::wookler::reactfs::common::__registry<_Key, _Type>::add(_Key key, _Type *type, bool configure) {
    PRECONDITION(NOT_NULL(type));
    std::lock_guard<std::mutex> lock(__lock);

    if (configure) {
        __configurable *c = dynamic_cast<__configurable *>(type);
        if (NOT_NULL(c) && NOT_NULL(config)) {
            c->configure(config);
        } else {
            throw BASE_ERROR("Instance cannot be configured.");
        }
    }
    registry.insert(make_pair(key, type));
}

template<class _Key, class _Type>
bool com::wookler::reactfs::common::__registry<_Key, _Type>::contains(_Key key) {
    typename unordered_map<_Key, _Type *>::const_iterator iter = registry.find(key);
    if (iter != registry.end()) {
        return true;
    }
    return false;
}

template<class _Key, class _Type>
_Type *com::wookler::reactfs::common::__registry<_Key, _Type>::get(_Key key) {
    typename unordered_map<_Key, _Type *>::const_iterator iter = registry.find(key);
    if (iter != registry.end()) {
        _Type *type = iter->second;
        CHECK_NOT_NULL(type);
        return type;
    }
    return nullptr;
}

template<class _Key, class _Type>
_Type *com::wookler::reactfs::common::__registry<_Key, _Type>::remove(_Key key) {
    std::lock_guard<std::mutex> lock(__lock);
    _Type *type = get(key);
    if (NOT_NULL(type)) {
        registry.erase(key);
    }
    return type;
}

template<class _Key, class _Type>
void com::wookler::reactfs::common::__registry<_Key, _Type>::clear() {
    std::lock_guard<std::mutex> lock(__lock);
    typename unordered_map<_Key, _Type *>::const_iterator iter;
    for (iter = registry.begin(); iter != registry.end(); iter++) {
        _Type *type = iter->second;
        CHECK_AND_FREE(type);
    }
    registry.clear();
}

#endif //REACTFS_REGISTRY_H
