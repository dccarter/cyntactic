//
// Created by Mpho Mbotho on 2021-08-16.
//

#include "symbols.hpp"
#include <exceptions.hpp>

namespace cyntactic {

    std::stack<SymTable> gScopes;
    SymTable &SymTable::get()
    {
        static bool initialized{false};
        if (!initialized) {
            gScopes.push({});
            initialized = true;
        }
        return gScopes.top();
    }

    void SymTable::push()
    {
        gScopes.push({&get()});
    }

    void SymTable::pop()
    {
        if (gScopes.size() == 1) {
            throw Exception("Cannot pop the global symbol table");
        }
        gScopes.pop();
    }

    bool SymTable::add(Symbol::Ptr &&sym)
    {
        auto& table = get();
        auto it = table.mSymbols.find(sym->Name);
        if (it == table.mSymbols.end()) {
            return false;
        }
        table.mSymbols.emplace(sym->Name, std::move(sym));
        return true;
    }

    Symbol::Ptr SymTable::get(const std::string_view& name)
    {
        auto& table = get();
        auto it = table.mSymbols.find(name);
        if (it == table.mSymbols.end()) {
            if (table.mParent != nullptr) {
                return table.mParent->get(name);
            }
            return nullptr;
        }
        return it->second;
    }

    bool SymTable::isDefined(const std::string_view &name)
    {
        auto& table = get();
        auto it = table.mSymbols.find(name);
        if (it == table.mSymbols.end()) {
            if (table.mParent != nullptr) {
                return table.mParent->isDefined(name);
            }
            return false;
        }
        return true;
    }
}