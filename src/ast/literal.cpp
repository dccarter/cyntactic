//
// Created by Mpho Mbotho on 2021-08-16.
//
#include <sstream>

#include "ast/literal.hpp"

namespace cyntactic::ast {

    std::string Literal::toString(bool compressed) const
    {
        std::string str{""};
        std::visit([&](const auto& v) {
            using T = std::remove_cvref_t<decltype(v)>;
            auto tmp = typeid(T).name();
            if constexpr (std::is_same_v<bool, T>) {
                str = v? "true" : "false";
            }
            else if constexpr (std::is_same_v<T, char>) {
                std::stringstream ss;
                ss << v;
                str = ss.str();
            }
            else if constexpr (is_integer<T> || std::is_floating_point_v<T>) {
                str = std::to_string(v);
            }
            else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                str = "null";
            }
            else {
                str = v;
            }
        }, mValue);

        return str;
    }
}