//
// Created by seeu on 2022/7/15.
//

#ifndef GOINTERPRETER_GIOBJECT_H
#define GOINTERPRETER_GIOBJECT_H

#include <string>
#include <sstream>
#include "Ast.h"
#include "Environment.h"

#define OBJECT_TYPE_MAP(name) map[GI::ObjectType::name] = #name

namespace GI {
    class Environment;

    enum class ObjectType {
        _NULL,
        ERROR,
        INTEGER,
        BOOLEAN,
        RETURN_VALUE,
        FUNCTION
    };

    class ObjectTypeMapping {
    public:
        ObjectTypeMapping() {

            OBJECT_TYPE_MAP(_NULL);
            OBJECT_TYPE_MAP(ERROR);
            OBJECT_TYPE_MAP(INTEGER);
            OBJECT_TYPE_MAP(BOOLEAN);
            OBJECT_TYPE_MAP(RETURN_VALUE);
            OBJECT_TYPE_MAP(FUNCTION);
        }

        std::map<ObjectType, std::string> map;
    };

    class GIObject {
    public:
        virtual ObjectType getType() = 0;


        virtual std::string inspect() = 0;

        virtual ~GIObject() = default;
    };

    class IntegerObject : public GIObject {
    public:
        IntegerObject(int value) : value{value} {}


        ObjectType getType() override { return ObjectType::INTEGER; }

        std::string inspect() override {
            return std::to_string(value);
        }

        int value;
    };

    class BooleanObject : public GIObject {
    public:
        BooleanObject(bool value) : value{value} {}

        ObjectType getType() override { return ObjectType::BOOLEAN; }

        std::string inspect() override { return std::to_string(value); }

        bool value;
    };

    class NullObject : public GIObject {
    public:
        ObjectType getType() override { return ObjectType::_NULL; }

        std::string inspect() override { return "null"; }
    };

    class ReturnValueObject : public GIObject {
    public:
        ReturnValueObject(std::unique_ptr<GIObject> value) : value{std::move(value)} {}

        ObjectType getType() override { return ObjectType::RETURN_VALUE; }

        std::string inspect() override { return value->inspect(); }

        std::unique_ptr<GIObject> value;
    };

    class ErrorObject : public GIObject {
    public:
        ErrorObject(std::string message) : message{message} {}

        ObjectType getType() override { return ObjectType::ERROR; }

        std::string inspect() override { return "Error: " + message; }

        std::string message;
    };

    class FunctionObject : public GIObject {
    public:
        ObjectType getType() override { return ObjectType::FUNCTION; }

        std::string inspect() override {
            std::stringstream ss;

            std::vector<std::string> params;
            for (auto &param: parameters) {
                params.push_back(param->toString());
            }
            std::stringstream paramsStream;
            std::copy(params.begin(), params.end() - 1, std::ostream_iterator<std::string>(paramsStream, ","));
            ss << "(" << paramsStream.str() << ")";
            ss << " {" << std::endl;
            ss << body->toString() << std::endl << "}";
            return ss.str();
        }

        std::vector<std::unique_ptr<Identifier>> parameters;
        std::unique_ptr<BlockStatement> body;
        std::unique_ptr<Environment> environment;
    };
}

#endif //GOINTERPRETER_GIOBJECT_H
