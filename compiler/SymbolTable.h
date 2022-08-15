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
        Local
    };

    struct Symbol {
        string name;
        SymbolScope scope;
        int index;
    };

    class SymbolTable {
    public:
        explicit SymbolTable(bool isGlobal) : isGlobal{isGlobal} {
        };

        SymbolTable() {}

        bool isGlobal{false};

        std::map<string, Symbol> store{};
        int numDefinitions{0};
    };

    class SymbolTableManager {
    public:
        SymbolTableManager() {
            // init default global table
            symbolTables.emplace_back(true);
        }

        void enterScope();

        void leaveScope();

        SymbolTable *symbolTable;

        Symbol define(const string &name);

        optional<Symbol> resolve(const string &name);

    private:
        std::vector<SymbolTable> symbolTables{};
    };

}

#endif //GOINTERPRETER_SYMBOLTABLE_H
