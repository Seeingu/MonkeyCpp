//
// Created by seeu on 2022/7/15.
//

#include "GIObject.h"

namespace Common {

    std::unique_ptr<BooleanObject> makeBoolObject(bool value) {
        return std::make_unique<BooleanObject>(value);
    }

    std::unique_ptr<ErrorObject> makeErrorObject(const std::string &message) {
        return std::make_unique<ErrorObject>(message);
    }
}
