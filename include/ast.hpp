//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <list>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <textbox.hpp>

namespace cyntactic::ast {

    struct Node {
        using Ptr = std::unique_ptr<Node>;
        using iterator = std::list<Ptr>::const_iterator;
        typedef enum {
            INVALID,
            PROGRAM,
            IDENT,
            IMPORT,
            LITERAL
        } Kind;

        Node() = default;
        Node(Kind kind) : Tag{kind} {}
        std::list<Ptr> Children;
        Kind   Tag{INVALID};
        virtual std::string toString(bool compressed = true) const { return ""; }
    };

    template <typename T>
        requires (std::is_base_of_v<Node, T> or std::is_same_v<T, Node>)
    struct ast_tag_t {
        static constexpr std::size_t value = typeid(T).hash_code();
    };

    template <typename T>
    constexpr std::size_t ast_tag = ast_tag_t<T>::value;

    class NumberType {
        struct Details {
            short  Size;
            bool   Signed{false};
            bool   Float{false};
        };

    public:
        NumberType(const std::string_view& tp);

        std::size_t size() const { return mDetails.Size; }
        const std::string_view& name() const { return mName; }
        bool isFloat() const { return mDetails.Float; }
        bool isSigned() const { return mDetails.Signed; }

    private:
        static const Details& getDetails(const std::string_view& tp, std::string_view& name);
        Details mDetails{};
        std::string_view mName{};
    };

    template <typename T>
    concept is_integer = std::is_same_v<T, int64_t>  ||
                         std::is_same_v<T, uint64_t> ||
                         std::is_same_v<T, int32_t>  ||
                         std::is_same_v<T, uint32_t> ||
                         std::is_same_v<T, int16_t>  ||
                         std::is_same_v<T, uint16_t> ||
                         std::is_same_v<T, uint8_t>;

    class Literal : public Node {
    public:
        Literal() : Node(Node::LITERAL) {}
        using Variant = std::variant<std::nullptr_t, bool, char, uint64_t, double, std::string>;

        template <typename T>
        requires is_integer<T>
        bool const is() { return std::holds_alternative<uint64_t>(mValue); }

        template<typename T>
        requires (!is_integer<T>)
        bool const is() { return std::holds_alternative<T>(mValue); }

        operator bool() const { return !std::holds_alternative<std::nullptr_t>(mValue); }

        template <typename T>
        requires is_integer<T>
        const T get() { return  static_cast<T&>(std::get<uint64_t>(mValue)); }

        template <typename T>
        requires (!is_integer<T>)
        const T& get() { return  std::get<T>(mValue); }
    private:
        Variant mValue{nullptr};
    };

    class Program : public Node {
    public:
        Program() : Node(Node::PROGRAM){};

    protected:
        std::string toString(bool compressed = true) const override;
    };

    class astIdent : public Node {
    public:
        astIdent() : Node(Node::IDENT) {}
        std::string Name{};
    protected:
        std::string toString(bool compressed = true) const override;
    };

    class astImport : public Node {
    public:
        astImport() : Node(Node::IMPORT){}
        std::string Name;
        std::string Identity;
        std::vector<std::string> Symbols;

    protected:
        std::string toString(bool compressed = true) const override;
    };
}


namespace cyntactic {

    template <>
    std::pair<ast::Node::iterator , ast::Node::iterator> TreeGraph<ast::Node>::countChildren() const;

    template <>
    bool TreeGraph<ast::Node>::isOneliner() const;

    template <>
    const ast::Node& TreeGraph<ast::Node>::getNode(const Iterator& it) const;

    template <>
    std::string TreeGraph<ast::Node>::createAtom() const;
}
