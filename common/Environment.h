//
// Created by seeu on 2022/7/15.
//

#ifndef GOINTERPRETER_ENVIRONMENT_H
#define GOINTERPRETER_ENVIRONMENT_H

#include <vector>
#include "GIObject.h"

namespace Common {
    class GIObject;

    class Environment {
    public:
        Environment(std::shared_ptr<Environment> outer) : outer{std::move(outer)} {}

        Environment() : outer{nullptr} {}

        void setValue(std::string name, std::shared_ptr<GIObject> value);

        std::shared_ptr<GIObject> getValue(std::string name);

        std::shared_ptr<Environment> outer;
        std::map<std::string, std::shared_ptr<GIObject>> store;

    };
}

#endif //GOINTERPRETER_ENVIRONMENT_H
