//
// Created by seeu on 2022/7/15.
//

#ifndef GOINTERPRETER_EVALUATOR_H
#define GOINTERPRETER_EVALUATOR_H

#include "Ast.h"
#include "Environment.h"
#include "Utils.h"

namespace GI {
    std::unique_ptr<GIObject> eval(
            Node *node,
            const std::shared_ptr<Environment> &environment
    );
}

#endif //GOINTERPRETER_EVALUATOR_H
