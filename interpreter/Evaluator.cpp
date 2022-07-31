//
// Created by seeu on 2022/7/15.
//

#include "Evaluator.h"
#include <typeindex>
#include <typeinfo>
#include <format>
#include <iostream>

namespace GI {
    static ObjectTypeMapping objectTypeNameMapping{};

    bool isError(GIObject *object) {
        return object != nullptr && object->getType() == ObjectType::ERROR;
    }

    bool isTruthy(GIObject *object) {
        if (object->getType() == ObjectType::BOOLEAN) {
            return static_cast<BooleanObject *>(object)->value;
        } else if (object->getType() == ObjectType::_NULL) {
            return false;
        } else {
            return true;
        }
    }

    std::unique_ptr<BooleanObject> makeBoolObject(bool value) {
        return std::make_unique<BooleanObject>(value);
    }

    std::unique_ptr<ErrorObject> makeErrorObject(std::string message) {
        return std::make_unique<ErrorObject>(message);
    }

    std::unique_ptr<GIObject> evalBlockStatement(BlockStatement *block, std::shared_ptr<Environment> environment) {
        std::unique_ptr<GIObject> result;
        for (auto &stmt: block->statements) {
            result = eval(stmt.get(), environment);
            if (result != nullptr) {
                if (result->getType() == ObjectType::RETURN_VALUE || result->getType() == ObjectType::ERROR) {
                    return result;
                }
            }
        }
        return result;
    }

    std::unique_ptr<GIObject> evalProgram(Program *program,
                                          std::shared_ptr<Environment> environment
    ) {
        std::unique_ptr<GIObject> result;
        for (auto &stmt: program->statements) {
            result = eval(stmt.get(), environment);
            auto returnValueObject = dynamic_cast<ReturnValueObject *>(result.get());
            if (returnValueObject != nullptr) {
                return std::move(returnValueObject->value);
            }
            auto errorObject = dynamic_cast<ErrorObject *>(result.get());
            if (errorObject != nullptr) {
                return result;
            }
        }
        return result;
    }

    std::unique_ptr<GIObject> evalBangOperatorExpression(std::unique_ptr<GIObject> object) {
        return std::make_unique<BooleanObject>(!isTruthy(object.get()));
    }

    std::unique_ptr<GIObject> evalMinusPrefixOperatorExpression(std::unique_ptr<GIObject> object) {
        if (object->getType() != ObjectType::INTEGER) {
            return std::make_unique<ErrorObject>("unknown operator: -" + objectTypeNameMapping.map[object->getType()]);
        }
        auto integerObject = static_cast<IntegerObject *>(object.get());
        return std::make_unique<IntegerObject>(-integerObject->value);
    }

    std::unique_ptr<GIObject> evalPrefixExpression(std::string prefixOperator, std::unique_ptr<GIObject> right) {
        if (prefixOperator == "!") {
            return evalBangOperatorExpression(std::move(right));
        } else if (prefixOperator == "-") {
            return evalMinusPrefixOperatorExpression(std::move(right));
        } else {
            return std::make_unique<ErrorObject>(
                    "unknown operator: " + prefixOperator + objectTypeNameMapping.map[right->getType()]);
        }
    }

    std::unique_ptr<GIObject> evalStringInfixExpression(std::string infixOperator, std::unique_ptr<GIObject> left,
                                                        std::unique_ptr<GIObject> right) {

        auto leftString = static_cast<StringObject *>(left.get());
        auto rightString = static_cast<StringObject *>(right.get());
        if (infixOperator == "+") {
            return std::make_unique<StringObject>(leftString->value + rightString->value);
        } else {
            std::stringstream ss;
            ss << "unknown infix operator with string: ";
            ss << leftString->value << " ";
            ss << infixOperator << " ";
            ss << rightString->value;
            return makeErrorObject(ss.str());
        }
    }

    std::unique_ptr<GIObject> evalIntegerInfixExpression(std::string infixOperator, std::unique_ptr<GIObject> left,
                                                         std::unique_ptr<GIObject> right) {
        auto leftInt = static_cast<IntegerObject *>(left.get());
        auto rightInt = static_cast<IntegerObject *>(right.get());

        if (infixOperator == "+") {
            return std::make_unique<IntegerObject>(leftInt->value + rightInt->value);
        } else if (infixOperator == "-") {
            return std::make_unique<IntegerObject>(leftInt->value - rightInt->value);
        } else if (infixOperator == "*") {
            return std::make_unique<IntegerObject>(leftInt->value * rightInt->value);
        } else if (infixOperator == "/") {
            return std::make_unique<IntegerObject>(leftInt->value / rightInt->value);
        } else if (infixOperator == "<") {
            return makeBoolObject(leftInt->value < rightInt->value);
        } else if (infixOperator == ">") {
            return makeBoolObject(leftInt->value > rightInt->value);
        } else if (infixOperator == "==") {
            return makeBoolObject(leftInt->value == rightInt->value);
        } else if (infixOperator == "!=") {
            return makeBoolObject(leftInt->value != rightInt->value);
        } else {
            std::stringstream ss;
            ss << "unknown infix operator with integer: ";
            ss << leftInt->value << " ";
            ss << infixOperator << " ";
            ss << rightInt->value;
            return makeErrorObject(ss.str());
        }
    }

    std::unique_ptr<GIObject>
    evalInfixExpression(std::string infixOperator, std::unique_ptr<GIObject> left, std::unique_ptr<GIObject> right) {
        if (left->getType() != right->getType()) {
            return std::make_unique<ErrorObject>(
                    "type mismatch: " + objectTypeNameMapping.map[left->getType()] + " " + infixOperator + " " +
                    objectTypeNameMapping.map[right->getType()]);
        }
        switch (left->getType()) {
            case ObjectType::INTEGER:
                return evalIntegerInfixExpression(infixOperator, std::move(left), std::move(right));

            case ObjectType::BOOLEAN: {
                if (infixOperator == "==") {
                    return makeBoolObject(static_cast<BooleanObject *>(left.get())->value ==
                                          static_cast<BooleanObject *>(right.get())->value);
                }
                if (infixOperator == "!=") {
                    return makeBoolObject(static_cast<BooleanObject *>(left.get())->value !=
                                          static_cast<BooleanObject *>(right.get())->value);
                }
            }
            case ObjectType::_NULL: {
                if (infixOperator == "==") {
                    return makeBoolObject(true);
                }
                if (infixOperator == "!=") {
                    return makeBoolObject(false);
                }
            }
            case ObjectType::STRING:
                return evalStringInfixExpression(infixOperator, std::move(left), std::move(right));
        }
        return makeErrorObject(
                "unknown operator: " + objectTypeNameMapping.map[left->getType()] + " " + infixOperator + " " +
                objectTypeNameMapping.map[right->getType()]);
    }

    std::unique_ptr<GIObject> evalIfExpression(IfExpression *node, std::shared_ptr<Environment> environment) {
        auto condition = eval(node->condition.get(), environment);
        if (isError(condition.get())) {
            return condition;
        }
        if (isTruthy(condition.get())) {
            return eval(node->consequence.get(), environment);
        } else if (node->alternative != nullptr) {
            return eval(node->alternative.get(), environment);
        }
        return nullptr;
    }

    std::unique_ptr<GIObject> evalIdentifierExpression(Identifier *node, std::shared_ptr<Environment> environment) {
        auto value = environment->getValue(node->value);
        if (value == nullptr) {
            return makeErrorObject("identifier not found: " + node->value);
        }
        switch (value->getType()) {
            case ObjectType::INTEGER:
                return std::make_unique<IntegerObject>(static_cast<IntegerObject *>(value.get())->value);
            case ObjectType::BOOLEAN:
                return std::make_unique<BooleanObject>(static_cast<BooleanObject *>(value.get())->value);
            case ObjectType::STRING:
                return std::make_unique<StringObject>(static_cast<StringObject *>(value.get())->value);
            case ObjectType::_NULL:
                return std::make_unique<NullObject>();
        }
        return makeErrorObject(
                "unsupported type get from env " + objectTypeNameMapping.map[value->getType()]);
    }

    std::unique_ptr<GIObject> evalBuiltinLen(std::vector<std::unique_ptr<GIObject>> arguments) {
        if (arguments.size() != 1) {
            return makeErrorObject("len() arguments size not match: " + std::to_string(arguments.size()));
        }
        auto arg = arguments[0].get();
        if (arg->getType() == ObjectType::STRING) {
            auto stringObject = static_cast<StringObject *>(arg);
            return std::make_unique<IntegerObject>(stringObject->value.size());
        } else {
            return makeErrorObject("len() argument type is not support: " + objectTypeNameMapping.map[arg->getType()]);
        }
    }

    std::vector<std::unique_ptr<GIObject>>
    evalFunctionArguments(CallExpression *node, std::shared_ptr<Environment> environment) {
        std::vector<std::unique_ptr<GIObject>> args{};
        for (auto &arg: node->arguments) {
            auto argObject = eval(arg.get(), environment);
            if (isError(argObject.get())) {
                args.push_back(std::move(argObject));
                break;
            }
            args.push_back(std::move(argObject));
        }
        return args;
    }

    std::unique_ptr<GIObject> evalCallExpression(CallExpression *node, std::shared_ptr<Environment> environment) {
        auto evalFunction = [&]( // NOLINT(misc-no-recursion)
                FunctionObject *functionObject) -> std::unique_ptr<GIObject> {
            auto args = evalFunctionArguments(node, environment);

            Environment env{functionObject->environment};
            if (functionObject->parameters.size() != args.size()) {
                return makeErrorObject("unexpected call arguments size");
            }
            for (int i = 0; i < functionObject->parameters.size(); i++) {
                env.setValue(functionObject->parameters[i]->value, std::move(args[i]));
            }

            auto result = eval(functionObject->body.get(), std::make_shared<Environment>(env));
            if (result->getType() == ObjectType::RETURN_VALUE) {
                return std::move(static_cast<ReturnValueObject *>(result.get())->value);
            }
            return result;
        };
        // iife
        if (node->name.get()->getType() != NodeType::Identifier) {
            auto object = eval(node->name.get(), environment);
            if (isError(object.get())) {
                return object;
            } else if (object->getType() != ObjectType::FUNCTION) {
                return makeErrorObject("unexpected call expression name");
            }
            auto functionObject = static_cast<FunctionObject *>(object.get());
            return evalFunction(functionObject);
        } else {
            auto identifier = static_cast<Identifier *>(node->name.get());
            auto fun = environment->getValue(identifier->value);
            if (fun == nullptr) {
                if (identifier->value == "len") {
                    auto args = evalFunctionArguments(node, environment);
                    return evalBuiltinLen(std::move(args));
                }
                return makeErrorObject("identifier not found: " + identifier->value);
            }
            if (fun->getType() != ObjectType::FUNCTION) {
                return makeErrorObject(objectTypeNameMapping.map[fun->getType()] + "is not a function");
            }
            auto functionObject = static_cast<FunctionObject *>(fun.get());
            return evalFunction(functionObject);
        }

    }

    std::unique_ptr<GIObject> evalExpression(Node *node, std::shared_ptr<Environment> environment) {
        switch (node->getType()) {
            case NodeType::IntegerExpression:
                return std::make_unique<IntegerObject>(static_cast<IntegerExpression *>(node)->value);
            case NodeType::BoolExpression:
                return std::make_unique<BooleanObject>(static_cast<BoolExpression *>(node)->value);
            case NodeType::StringExpression:
                return std::make_unique<StringObject>(static_cast<StringExpression *>(node)->value);
            case NodeType::PrefixExpression: {
                auto prefixExpression = static_cast<PrefixExpression *>(node);
                auto right = eval(prefixExpression->rightExpression.get(), environment);
                if (isError(right.get())) {
                    return right;
                }
                return evalPrefixExpression(prefixExpression->prefixOperator, std::move(right));
            }
            case NodeType::InfixExpression: {
                auto infixExpression = static_cast<InfixExpression *>(node);
                auto left = eval(infixExpression->leftExpression.get(), environment);
                if (isError(left.get())) {
                    return left;
                }
                auto right = eval(infixExpression->rightExpression.get(), environment);
                if (isError(right.get())) {
                    return right;
                }
                return evalInfixExpression(infixExpression->infixOperator, std::move(left), std::move(right));
            }
            case NodeType::IfExpression:
                return evalIfExpression(static_cast<IfExpression *>(node), environment);
            case NodeType::Identifier:
                return evalIdentifierExpression(static_cast<Identifier *>(node), environment);
            case NodeType::FunctionExpression: {
                auto fnExpression = static_cast<FunctionExpression *>(node);
                return std::make_unique<FunctionObject>(
                        std::move(fnExpression->parameters),
                        std::move(fnExpression->body), environment);
            }
            case NodeType::CallExpression:
                return evalCallExpression(static_cast<CallExpression *>(node), environment);
        }
        return nullptr;
    }

    std::unique_ptr<GIObject> eval(
            Node *node,
            std::shared_ptr<Environment> environment
    ) {
        switch (node->getType()) {
            case NodeType::Program:
                return evalProgram(static_cast<Program *>(node), environment);
            case NodeType::BlockStatement:
                return evalBlockStatement(static_cast<BlockStatement *>(node), environment);
            case NodeType::ExpressionStatement: {
                return eval(static_cast<ExpressionStatement *>(node)->expression.get(), environment);
            }
            case NodeType::ReturnStatement: {
                auto value = eval(static_cast<ReturnStatement *>(node)->returnValue.get(), environment);
                if (isError(value.get())) {
                    return value;
                }
                return std::make_unique<ReturnValueObject>(std::move(value));
            }
            case NodeType::LetStatement: {
                auto letStatement = static_cast<LetStatement *>(node);
                auto value = eval(letStatement->value.get(), environment);
                if (isError(value.get())) {
                    return value;
                }
                environment.get()->setValue(letStatement->name->value, std::move(value));
                return nullptr;
            }
            default:
                return evalExpression(node, environment);
        }
        return nullptr;

    }
}
