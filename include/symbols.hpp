//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <stack>

namespace cyntactic {

    struct Symbol {
        using Ptr = std::shared_ptr<Symbol>;
        typedef enum {
            S_IDENT,
            S_FUNC,
            S_LAMBDA,
            S_TYPE,
            S_MODULE
        } Kind;
        Kind kind{};
        std::string Name{};
    };

    class SymTable {
    public:
        using Ptr = std::unique_ptr<SymTable>;

        template <typename T, typename... Args>
        static bool add(std::string name, Args... args);
        static bool add(Symbol::Ptr&& sym);
        static void push();
        static void pop();
        static Symbol::Ptr get(const std::string_view& name);
        static bool isDefined(const std::string_view& name);
    private:
        SymTable(SymTable* parent = nullptr)
            : mParent{parent}
        {}
        static SymTable& get();
        std::unordered_map<std::string_view, Symbol::Ptr> mSymbols{};
        SymTable *mParent{nullptr};
    };

    template <typename T, typename... Args>
    bool SymTable::add(std::string name, Args... args)
    {
        return add(std::make_unique<T>(std::move(name), std::forward<Args>(args)...));
    }
}