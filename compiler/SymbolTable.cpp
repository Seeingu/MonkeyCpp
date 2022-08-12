//
// Created by seeu on 2022/8/11.
//

#include "SymbolTable.h"

namespace GC {

    Symbol SymbolTable::define(const string &name) {
        auto s = Symbol{name, GLOBAL_SCOPE, numDefinitions};
        store[name] = s;
        numDefinitions++;
        return s;
    }

    optional<Symbol> SymbolTable::resolve(const string &name) {
        if (store.contains(name)) {
            return store[name];
        }
        return nullopt;
    }
}
