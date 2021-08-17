//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <variant>

#include <node.hpp>

namespace cyntactic::ast {

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

        template <typename T>
        Literal(T&& value) : Node(Node::LITERAL), mValue{std::forward<T>(value)}
        {}

        using Variant = std::variant<std::nullptr_t, bool, char, uint64_t, double, std::string>;

        template <typename T>
        requires is_integer<T>
        bool const is() const { return std::holds_alternative<uint64_t>(mValue); }

        template<typename T>
        requires (!is_integer<T>)
        bool const is() const { return std::holds_alternative<T>(mValue); }

        operator bool() const { return !std::holds_alternative<std::nullptr_t>(mValue); }

        template <typename T>
        requires is_integer<T>
        const T get() { return  static_cast<T&>(std::get<uint64_t>(mValue)); }

        template <typename T>
        requires (!is_integer<T>)
        const T& get() { return  std::get<T>(mValue); }

        std::string toString(bool compressed = true) const override;

    private:
        Variant mValue{nullptr};
    };
}
