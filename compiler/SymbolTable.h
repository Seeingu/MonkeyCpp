//
// Created by seeu on 2022/8/11.
//

#ifndef GOINTERPRETER_SYMBOLTABLE_H
#define GOINTERPRETER_SYMBOLTABLE_H

#include <map>
#include <string>

#define GLOBAL_SCOPE "GLOBAL_SCOPE"

namespace GC {
    using namespace std;
    using SymbolScope = string;

    struct Symbol {
        string name;
        SymbolScope scope;
        int index;
    };

    class SymbolTable {
    public:
        Symbol define(const string &name);

        optional<Symbol> resolve(const string &name);

    private:
        std::map<string, Symbol> store;
        int numDefinitions;
    };

}

#endif //GOINTERPRETER_SYMBOLTABLE_H
