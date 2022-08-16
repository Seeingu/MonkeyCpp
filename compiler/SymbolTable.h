//
// Created by seeu on 2022/8/11.
//

#ifndef GOINTERPRETER_SYMBOLTABLE_H
#define GOINTERPRETER_SYMBOLTABLE_H

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace GC {
    using namespace std;
    enum class SymbolScope {
        Global,
        Local,
        Builtin,
        Function,
        Free
    };

    struct Symbol {
        string name;
        SymbolScope scope;
        int index;
    };

    class SymbolTable {
    public:
        SymbolTable(bool hasOuter) : hasOuter{hasOuter} {}

        std::map<string, Symbol> store{};
        int numDefinitions{0};
        std::vector<Symbol> freeSymbols{};

        bool hasOuter;
    };

    class SymbolTableManager {
    public:
        SymbolTableManager() {
            // init default global table
            symbolTables.emplace_back(false);
        }

        void enterScope();

        void leaveScope();

        SymbolTable *symbolTable;

        Symbol define(const string &name);

        Symbol defineBuiltin(int index, string name);

        Symbol defineFunctionName(string name);

        Symbol defineFree(Symbol original);

        std::optional<Symbol> resolve(const string &name);

    private:
        std::vector<SymbolTable> symbolTables{};
    };

}

#endif //GOINTERPRETER_SYMBOLTABLE_H
