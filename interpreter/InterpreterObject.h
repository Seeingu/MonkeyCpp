//
// Created by seeu on 2022/8/20.
//

#ifndef MONKEYREPL_INTERPRETEROBJECT_H
#define MONKEYREPL_INTERPRETEROBJECT_H

#include "GIObject.h"
#include "Environment.h"

namespace GI {
    using namespace Common;

    struct FunctionObject : GIObject {
        FunctionObject(
                std::vector<std::unique_ptr<Identifier>> parameters,
                std::unique_ptr<BlockStatement> body,
                std::shared_ptr<Environment> environment
        ) : parameters{std::move(parameters)}, body{std::move(body)}, environment{std::move(environment)} {}

        ObjectType getType() override { return ObjectType::FUNCTION; }

        std::string inspect() override {
            std::stringstream ss;

            std::vector<std::string> params;
            for (auto &param: parameters) {
                params.push_back(param->toString());
            }
            std::stringstream paramsStream;
            std::copy(params.begin(), params.end() - 1, std::ostream_iterator<std::string>(paramsStream, ", "));
            ss << "(" << paramsStream.str() << ")";
            ss << " {" << std::endl;
            ss << body->toString() << std::endl << "}";
            return ss.str();
        }

        std::vector<std::unique_ptr<Identifier>> parameters;
        std::unique_ptr<BlockStatement> body;
        std::shared_ptr<Environment> environment;
    };
}


#endif //MONKEYREPL_INTERPRETEROBJECT_H
