//
// Created by seeu on 2022/8/14.
//

#include "Builtin.h"
#include "fmt/format.h"
#include "magic_enum.hpp"

namespace Common {
    std::unique_ptr<GIObject> evalBuiltinLen(BuiltinArguments arguments) {
        if (arguments.size() != 1) {
            return makeErrorObject("len() arguments size not match: " + std::to_string(arguments.size()));
        }
        auto arg = arguments[0].get();
        switch (arg->getType()) {
            case ObjectType::STRING: {
                auto stringObject = static_cast<StringObject *>(arg);
                return std::make_unique<IntegerObject>(stringObject->value.size());
            }
            case ObjectType::ARRAY: {
                auto arrayObject = static_cast<ArrayObject *>(arg);
                return std::make_unique<IntegerObject>(arrayObject->elements.size());
            }
            default:
                return makeErrorObject(
                        fmt::format("len() argument type is not support: {}", magic_enum::enum_name(arg->getType())));
        }
    }

    std::unique_ptr<GIObject> evalBuiltin(string name, BuiltinArguments args) {
        if (name == "len") {
            return evalBuiltinLen(args);
        }
        return nullptr;
    }

}
