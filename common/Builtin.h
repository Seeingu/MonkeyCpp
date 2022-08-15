//
// Created by seeu on 2022/8/14.
//

#ifndef GOINTERPRETER_BUILTIN_H
#define GOINTERPRETER_BUILTIN_H

#include "GIObject.h"

namespace Common {
    using namespace std;

    using BuiltinArguments = std::vector<std::shared_ptr<GIObject>>;

    std::unique_ptr<GIObject> evalBuiltin(string name, BuiltinArguments args);

}


#endif //GOINTERPRETER_BUILTIN_H
