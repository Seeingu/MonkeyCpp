//
// Created by seeu on 2022/7/9.
//

#ifndef GOINTERPRETER_AST_H
#define GOINTERPRETER_AST_H


#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "Token.h"

using namespace std;

namespace GI {
    enum class NodeType {
        Identifier,
        IntegerExpression,
        StringExpression,
        ArrayExpression,
        IndexExpression,
        HashExpression,
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

    struct Node {
        virtual std::string toString() = 0;

        virtual NodeType getType() = 0;

        virtual ~Node() = default;
    };

    struct Expression : Node {
        std::string toString() override = 0;

        NodeType getType() override = 0;
    };

    struct Identifier : Expression {
        Identifier(Token token, std::string value) : token{std::move(token)}, value{value} {};

        NodeType getType() override { return NodeType::Identifier; };

        std::string toString() override;

        Token token;
        std::string value;
    };

    struct IntegerExpression : Expression {
        IntegerExpression(Token token, int value) : token{std::move(token)}, value{value} {}

        NodeType getType() override { return NodeType::IntegerExpression; };

        std::string toString() override {
            return token.literal;
        }

        Token token;
        int value;
    };

    struct StringExpression : Expression {
        StringExpression(Token token, std::string value) : token{std::move(token)}, value{std::move(value)} {}

        NodeType getType() override { return NodeType::StringExpression; }

        std::string toString() override {
            return value;
        }

        Token token;
        std::string value;
    };

    struct ArrayExpression : Expression {
        ArrayExpression(Token token, std::vector<std::unique_ptr<Expression>> elements) :
                token{std::move(token)},
                elements{std::move(elements)} {}

        NodeType getType() override { return NodeType::ArrayExpression; }

        std::string toString() override;

        Token token;
        std::vector<std::unique_ptr<Expression>> elements;
    };

    struct HashExpression : Expression {
        HashExpression(Token token, std::map<unique_ptr<Expression>,unique_ptr<Expression>> pairs) : token{token}, pairs{std::move(pairs)} {}

        NodeType getType() override { return NodeType::HashExpression;};

        string toString() override;

        Token token;
        std::map<unique_ptr<Expression>,unique_ptr<Expression> > pairs;
    };

    struct IndexExpression : Expression {
        IndexExpression(Token token, unique_ptr <Expression> leftExpression,
                        unique_ptr <Expression> indexExpression) :
                token(std::move(token)), leftExpression(std::move(leftExpression)),
                indexExpression(std::move(indexExpression)) {}

        std::string toString() override {
            return "";
        }

        NodeType getType() override {
            return NodeType::IndexExpression;
        }

        Token token;
        unique_ptr <Expression> leftExpression;
        unique_ptr <Expression> indexExpression;
    };


    struct PrefixExpression : Expression {
        PrefixExpression(Token token, std::string prefixOperator, std::unique_ptr<Expression> rightExpression) : token{
                std::move(
                        token)}, prefixOperator{prefixOperator}, rightExpression{std::move(rightExpression)} {};

        std::string toString() override;

        NodeType getType() override { return NodeType::PrefixExpression; };

        Token token;
        std::string prefixOperator;
        std::unique_ptr<Expression> rightExpression;
    };

    struct BoolExpression : Expression {
        BoolExpression(Token token, bool value) : token(std::move(token)), value{value} {}

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
    struct BlockStatement;

    struct IfExpression : Expression {
        IfExpression(Token token,
                     std::unique_ptr<Expression> condition,
                     std::unique_ptr<BlockStatement> consequences,
                     std::unique_ptr<BlockStatement> alternative
        ) : token{std::move(token)}, condition{std::move(condition)}, consequence{std::move(consequences)},
            alternative{std::move(alternative)} {}

        std::string toString() override;

        NodeType getType() override { return NodeType::IfExpression; };

        Token token;
        std::unique_ptr<Expression> condition;
        std::unique_ptr<BlockStatement> consequence;
        std::unique_ptr<BlockStatement> alternative;
    };

    struct FunctionExpression : Expression {
        FunctionExpression(Token token,
                           std::vector<std::unique_ptr<Identifier>> parameters,
                           std::unique_ptr<BlockStatement> body
        ) : token{std::move(token)}, parameters{std::move(parameters)}, body{std::move(body)} {}

        NodeType getType() override { return NodeType::FunctionExpression; };

        std::string toString() override;

        Token token;
        std::vector<std::unique_ptr<Identifier>> parameters;
        std::unique_ptr<BlockStatement> body;
    };

    struct Statement : Node {
        std::string toString() override = 0;

        NodeType getType() override = 0;

        ~Statement() override = default;
    };

    struct LetStatement : Statement {
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

    struct BlockStatement : Statement {
        BlockStatement(Token token,
                       std::vector<std::unique_ptr<Statement>> statements) :
                token{std::move(token)},
                statements{std::move(statements)} {}

        NodeType getType() override { return NodeType::BlockStatement; };

        std::string toString() override;

        Token token;
        std::vector<std::unique_ptr<Statement>> statements;
    };

    struct ReturnStatement : Statement {
        ReturnStatement(Token token, std::unique_ptr<Expression> returnValue) : token{
                std::move(token)}, returnValue{std::move(returnValue)} {};

        NodeType getType() override { return NodeType::ReturnStatement; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> returnValue;
    };

    struct ExpressionStatement : Statement {
        ExpressionStatement(
                Token token,
                std::unique_ptr<Expression> expression
        ) : token{std::move(token)}, expression{std::move(expression)} {}

        NodeType getType() override { return NodeType::ExpressionStatement; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> expression;
    };

    struct InfixExpression : Expression {
        InfixExpression(Token token,
                        std::unique_ptr<Expression> leftExpression,
                        std::unique_ptr<Expression> rightExpression,
                        std::string infixOperator
        ) : token(std::move(token)), leftExpression{std::move(leftExpression)},
            rightExpression{std::move(rightExpression)},
            infixOperator{std::move(infixOperator)} {};

        NodeType getType() override { return NodeType::InfixExpression; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> leftExpression;
        std::unique_ptr<Expression> rightExpression;
        std::string infixOperator;
    };

    struct CallExpression : Expression {
        CallExpression(Token token,
                       std::unique_ptr<Expression> name,
                       std::vector<std::unique_ptr<Expression>> arguments
        ) : token{std::move(token)}, name{std::move(name)}, arguments{std::move(arguments)} {};

        NodeType getType() override { return NodeType::CallExpression; };

        std::string toString() override;

        Token token;
        std::unique_ptr<Expression> name;
        std::vector<std::unique_ptr<Expression>> arguments;
    };

    struct Program : Node {
        explicit Program(std::vector<std::unique_ptr<Statement>> statements) : statements{std::move(statements)} {}

        NodeType getType() override { return NodeType::Program; };

        std::string toString() override;

        std::vector<std::unique_ptr<Statement>> statements;
    };

}

#endif //GOINTERPRETER_AST_H
