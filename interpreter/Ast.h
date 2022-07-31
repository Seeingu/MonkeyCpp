//
// Created by seeu on 2022/7/9.
//

#ifndef GOINTERPRETER_AST_H
#define GOINTERPRETER_AST_H


#include <string>
#include <vector>
#include <memory>
#include "Token.h"

namespace GI {
    enum class NodeType {
        Identifier,
        IntegerExpression,
        StringExpression,
        PrefixExpression,
        BoolExpression,
        IfExpression,
        FunctionExpression,
        LetStatement,
        BlockStatement,
        ReturnStatement,
        ExpressionStatement,
        InfixExpression,
        CallExpression,
        Program
    };

    class Node {
    public:
        virtual std::string toString() = 0;

        virtual NodeType getType() = 0;

        virtual ~Node() = default;
    };

    class Expression : public Node {
    public:
        virtual std::string toString() = 0;

        virtual NodeType getType() = 0;
    };

    class Identifier : public Expression {
    public:
        Identifier(Token token, std::string value) : token{token}, value{value} {};

        NodeType getType() override { return NodeType::Identifier; };

        std::string toString() override;

        Token token;
        std::string value;
    };

    class IntegerExpression : public Expression {
    public:
        IntegerExpression(Token token, int value) : token{std::move(token)}, value{value} {}

        NodeType getType() override { return NodeType::IntegerExpression; };

        std::string toString() {
            return token.literal;
        }

        Token token;
        int value;
    };

    class StringExpression : public Expression {
    public:
        StringExpression(Token token, std::string value) : token{std::move(token)}, value{value} {}

        NodeType getType() override { return NodeType::StringExpression; }

        std::string toString() override {
            return value;
        }

        Token token;
        std::string value;
    };

    class PrefixExpression : public Expression {
    public:
        PrefixExpression(Token token, std::string prefixOperator, std::unique_ptr<Expression> rightExpression) : token{
                token}, prefixOperator{prefixOperator}, rightExpression{std::move(rightExpression)} {};

        std::string toString() override;

        NodeType getType() override { return NodeType::PrefixExpression; };

        Token token;
        std::string prefixOperator;
        std::unique_ptr<Expression> rightExpression;
    };

    class BoolExpression : public Expression {
    public:
        BoolExpression(Token token, bool value) : token(token), value{value} {}

        std::string toString() override {
            return token.literal;
        };

        NodeType getType() override {
            return NodeType::BoolExpression;
        }

        Token token;
        bool value;
    };

    // predeclare
    class BlockStatement;

    class IfExpression : public Expression {
    public:
        IfExpression(Token token,
                     std::unique_ptr<Expression> condition,
                     std::unique_ptr<BlockStatement> consequences,
                     std::unique_ptr<BlockStatement> alternative
        ) : token{token}, condition{std::move(condition)}, consequence{std::move(consequences)},
            alternative{std::move(alternative)} {}

        std::string toString() override;

        NodeType getType() override { return NodeType::IfExpression; };

        Token token;
        std::unique_ptr<Expression> condition;
        std::unique_ptr<BlockStatement> consequence;
        std::unique_ptr<BlockStatement> alternative;
    };

    class FunctionExpression : public Expression {
    public:
        FunctionExpression(Token token,
                           std::vector<std::unique_ptr<Identifier>> parameters,
                           std::unique_ptr<BlockStatement> body
        ) : token{token}, parameters{std::move(parameters)}, body{std::move(body)} {}

        NodeType getType() override { return NodeType::FunctionExpression; };

        std::string toString() override;

        Token token;
        std::vector<std::unique_ptr<Identifier>> parameters;
        std::unique_ptr<BlockStatement> body;
    };

    class Statement : public Node {
    public:
        virtual std::string toString() = 0;

        virtual NodeType getType() = 0;

        virtual ~Statement() = default;
    };

    class LetStatement : public Statement {
    public:
        LetStatement(Token token, std::unique_ptr<Identifier> name, std::unique_ptr<Expression> value)
                : token{std::move(token)}, name{std::move(name)}, value{std::move(value)} {}

        std::string toString() override;

        NodeType getType() override {
            return NodeType::LetStatement;
        }

        Token token;
        std::unique_ptr<Identifier> name;
        std::unique_ptr<Expression> value;
    };

    class BlockStatement : public Statement {
    public:
        BlockStatement(Token token,
                       std::vector<std::unique_ptr<Statement>> statements) :
                token{token},
                statements{std::move(statements)} {}

        NodeType getType() override { return NodeType::BlockStatement; };

        std::string toString() override;

        Token token;
        std::vector<std::unique_ptr<Statement>> statements;
    };

    class ReturnStatement : public Statement {
    public:
        ReturnStatement(Token token, std::unique_ptr<Expression> returnValue) : token{
                std::move(token)}, returnValue{std::move(returnValue)} {};

        NodeType getType() override { return NodeType::ReturnStatement; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> returnValue;
    };

    class ExpressionStatement : public Statement {
    public:
        ExpressionStatement(
                Token token,
                std::unique_ptr<Expression> expression
        ) : token{std::move(token)}, expression{std::move(expression)} {}

        NodeType getType() override { return NodeType::ExpressionStatement; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> expression;
    };

    class InfixExpression : public Expression {
    public:
        InfixExpression(Token token,

                        std::unique_ptr<Expression> leftExpression,
                        std::unique_ptr<Expression> rightExpression,
                        std::string infixOperator
        ) : token(token), leftExpression{std::move(leftExpression)}, rightExpression{std::move(rightExpression)},
            infixOperator{infixOperator} {};

        NodeType getType() override { return NodeType::InfixExpression; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> leftExpression;
        std::unique_ptr<Expression> rightExpression;
        std::string infixOperator;
    };

    class CallExpression : public Expression {
    public:
        CallExpression(Token token,
                       std::unique_ptr<Expression> name,
                       std::vector<std::unique_ptr<Expression>> arguments
        ) : token{token}, name{std::move(name)}, arguments{std::move(arguments)} {};

        NodeType getType() override { return NodeType::CallExpression; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> name;
        std::vector<std::unique_ptr<Expression>> arguments;
    };

    class Program : public Node {
    public:
        explicit Program(std::vector<std::unique_ptr<Statement>> statements) : statements{std::move(statements)} {}

        NodeType getType() override { return NodeType::Program; };

        std::string toString() override;

        std::vector<std::unique_ptr<Statement>> statements;
    };

}

#endif //GOINTERPRETER_AST_H
