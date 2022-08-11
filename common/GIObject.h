//
// Created by seeu on 2022/7/15.
//

#ifndef GOINTERPRETER_GIOBJECT_H
#define GOINTERPRETER_GIOBJECT_H

#include <string>
#include <sstream>
#include <iterator>
#include "Ast.h"
#include "Environment.h"

#define OBJECT_TYPE_MAP(name) map[Common::ObjectType::name] = #name

namespace Common {
    class Environment;

    using HashKey = std::size_t;

    enum class ObjectType {
        _NULL,
        ERROR,
        INTEGER,
        BOOLEAN,
        STRING,
        BUILTIN,
        RETURN_VALUE,
        FUNCTION,
        ARRAY,
        HASH
    };

    class ObjectTypeMapping {
    public:
        ObjectTypeMapping() {

            OBJECT_TYPE_MAP(_NULL);
            OBJECT_TYPE_MAP(ERROR);
            OBJECT_TYPE_MAP(INTEGER);
            OBJECT_TYPE_MAP(STRING);
            OBJECT_TYPE_MAP(BOOLEAN);
            OBJECT_TYPE_MAP(RETURN_VALUE);
            OBJECT_TYPE_MAP(FUNCTION);
            OBJECT_TYPE_MAP(BUILTIN);
            OBJECT_TYPE_MAP(ARRAY);
            OBJECT_TYPE_MAP(HASH);
        }

        std::map<ObjectType, std::string> map;
    };

    struct GIObject {
        virtual ObjectType getType() = 0;

        virtual std::string inspect() = 0;

        virtual ~GIObject() = default;

        virtual HashKey hash() {
            throw "hash is not supported";
        }
    };

    struct IntegerObject : GIObject {
        explicit IntegerObject(int value) : value{value} {}

        ObjectType getType() override { return ObjectType::INTEGER; }

        std::string inspect() override {
            return std::to_string(value);
        }

        HashKey hash() override {
            return std::hash<int>{}(value);
        }

        int value;
    };

    struct StringObject : GIObject {
        explicit StringObject(std::string value) : value{value} {}

        ObjectType getType() override { return ObjectType::STRING; }

        std::string inspect() override { return value; }

        std::string value;

        HashKey hash() override {
            return std::hash<string>{}(value);
        }
    };

    struct BooleanObject : GIObject {
        explicit BooleanObject(bool value) : value{value} {}

        ObjectType getType() override { return ObjectType::BOOLEAN; }

        std::string inspect() override { return std::to_string(value); }

        HashKey hash() override {
            return std::hash<bool>{}(value);

        }

        bool value;
    };

    struct NullObject : GIObject {
        ObjectType getType() override { return ObjectType::_NULL; }

        std::string inspect() override { return "null"; }
    };

    struct ReturnValueObject : GIObject {
        explicit ReturnValueObject(std::shared_ptr<GIObject> value) : value{std::move(value)} {}

        ObjectType getType() override { return ObjectType::RETURN_VALUE; }

        std::string inspect() override { return value->inspect(); }

        std::shared_ptr<GIObject> value;
    };

    struct ErrorObject : GIObject {
        ErrorObject(std::string message) : message{message} {}

        ObjectType getType() override { return ObjectType::ERROR; }

        std::string inspect() override { return "Error: " + message; }

        std::string message;
    };

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
            std::copy(params.begin(), params.end() - 1, std::ostream_iterator<std::string>(paramsStream, ","));
            ss << "(" << paramsStream.str() << ")";
            ss << " {" << std::endl;
            ss << body->toString() << std::endl << "}";
            return ss.str();
        }

        std::vector<std::unique_ptr<Identifier>> parameters;
        std::unique_ptr<BlockStatement> body;
        std::shared_ptr<Environment> environment;
    };

    struct BuiltinFunctionObject : GIObject {
        BuiltinFunctionObject(
                std::vector<std::unique_ptr<Identifier>> parameters,
                std::unique_ptr<BlockStatement> body,
                std::shared_ptr<Environment> environment
        ) : parameters{std::move(parameters)}, body{std::move(body)}, environment{std::move(environment)} {}

        ObjectType getType() override { return ObjectType::BUILTIN; }

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
        std::shared_ptr<Environment> environment;
    };

    struct ArrayObject : GIObject {
        explicit ArrayObject(std::vector<std::shared_ptr<GIObject>> elements) : elements(std::move(elements)) {}

        ObjectType getType() override {
            return ObjectType::ARRAY;
        }

        std::string inspect() override {
            std::stringstream ss;

            std::vector<std::string> elems;
            for (auto &elem: elements) {
                elems.push_back(elem->inspect());
            }
            std::stringstream elemsStream;
            std::copy(elems.begin(), elems.end(), std::ostream_iterator<std::string>(elemsStream, ", "));
            ss << "[" << elemsStream.str() << "]";
            return ss.str();
        }

        std::vector<std::shared_ptr<GIObject>> elements;
    };

    struct HashPair {
        std::shared_ptr<GIObject> key;
        std::shared_ptr<GIObject> value;
    };

    struct HashObject : GIObject {
        explicit HashObject(std::map<HashKey, HashPair> pairs) : pairs{std::move(pairs)} {}

        ObjectType getType() override { return ObjectType::HASH; };

        std::string inspect() override {
            ostringstream ss;
            ss << "{";
            std::vector<string> pairList;
            for (auto &p: pairs) {
                pairList.push_back(p.second.key->inspect() + ": " + p.second.value->inspect());
            }
            ostringstream pairStream;
            std::copy(pairList.begin(), pairList.end(), std::ostream_iterator<string>(pairStream, ", "));
            ss << pairStream.str();
            ss << "}";
            return ss.str();
        }

        std::map<HashKey, HashPair> pairs;
    };
}

#endif //GOINTERPRETER_GIOBJECT_H
