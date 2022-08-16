//
// Created by seeu on 2022/8/11.
//

#include "SymbolTable.h"

namespace GC {
    void SymbolTableManager::enterScope() {
        symbolTables.emplace_back(true);
        symbolTable = &symbolTables.back();
    }

    void SymbolTableManager::leaveScope() {
        symbolTables.pop_back();
        symbolTable = &symbolTables.back();
    }

    Symbol SymbolTableManager::define(const string &name) {
        auto st = &symbolTables.back();
        auto s = Symbol{name, SymbolScope::Global, st->numDefinitions};
        if (st->hasOuter) {
            s.scope = SymbolScope::Local;
        }
        st->store[name] = s;
        st->numDefinitions++;
        return s;
    }


    Symbol SymbolTableManager::defineBuiltin(int index, string name) {
        auto s = Symbol{name, SymbolScope::Builtin, index};
        auto st = &symbolTables.back();
        st->store[name] = s;
        return s;
    }

    Symbol SymbolTableManager::defineFunctionName(string name) {
        auto s = Symbol{name, SymbolScope::Function, 0};
        auto st = &symbolTables.back();
        st->store[name] = s;
        return s;
    }

    Symbol SymbolTableManager::defineFree(Symbol original) {
        auto st = &symbolTables.back();
        st->freeSymbols.push_back(original);

        auto s = Symbol{original.name, SymbolScope::Free, int(st->freeSymbols.size()) - 1};
        st->store[original.name] = s;
        return s;
    }

    optional<Symbol> SymbolTableManager::resolve(const string &name) {
        auto localSymbolTable = symbolTables.back();
        if (localSymbolTable.store.contains(name)) {
            return localSymbolTable.store[name];
        }
        for (auto s = symbolTables.rbegin() + 1; s != symbolTables.rend(); s++) {
            if (s->store.contains(name)) {
                auto symbol = s->store[name];

                if (symbol.scope == SymbolScope::Global || symbol.scope == SymbolScope::Builtin) {
                    return symbol;
                }
                return defineFree(symbol);
            }
        }
        return nullopt;
    }
}
