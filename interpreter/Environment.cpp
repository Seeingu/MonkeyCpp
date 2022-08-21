//
// Created by seeu on 2022/7/15.
//

#include "Environment.h"

namespace Common {
    void Environment::setValue(std::string name, std::shared_ptr<GIObject> value) {
        store[name] = std::move(value);
    }

    std::shared_ptr<GIObject> Environment::getValue(std::string name) {
        if (store.contains(name)) {
            return store[name];
        }
        return outer != nullptr ? outer->getValue(name) : nullptr;
    }
}
