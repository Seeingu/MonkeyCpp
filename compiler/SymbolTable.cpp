//
// Created by seeu on 2022/8/11.
//

#include "SymbolTable.h"

namespace GC {
    void SymbolTableManager::enterScope() {
        symbolTables.emplace_back();
        symbolTable = &symbolTables.back();
    }

    void SymbolTableManager::leaveScope() {
        symbolTables.pop_back();
        symbolTable = &symbolTables.back();
    }

    Symbol SymbolTableManager::define(const string &name) {
        auto st = &symbolTables.back();
        auto s = Symbol{name, SymbolScope::Global, st->numDefinitions};
        if (!st->isGlobal) {
            s.scope = SymbolScope::Local;
        }
        st->store[name] = s;
        st->numDefinitions++;
        return s;
    }

    optional<Symbol> SymbolTableManager::resolve(const string &name) {
        for (auto s = symbolTables.rbegin(); s != symbolTables.rend(); s++) {
            if (s->store.contains(name)) {
                return s->store[name];
            }
        }
        return nullopt;
    }
}
