//
// Created by Mpho Mbotho on 2021-08-16.
//

#include "ast/number.hpp"

namespace cyntactic::ast {

    NumberType::NumberType(const std::string_view &tp)
    : Node(Node::NUMBER_TYPE),
      mDetails{getDetails(tp, mName)}
    {}

    const NumberType::Details& NumberType::getDetails(const std::string_view &tp, std::string_view& name)
    {
        static const std::unordered_map<std::string_view, Details> NumTypeDetails {
            {"",        Details{0, false, false}},
            {"i8",      Details{1, true}},
            {"u8",      Details{1, false}},
            {"i16",     Details{2, true}},
            {"u16",     Details{2, false}},
            {"i32",     Details{4, true}},
            {"u32",     Details{4, false}},
            {"i64",     Details{8, true}},
            {"u64",     Details{8, false}},
            {"f32",     Details{4, true, true}},
            {"f64",     Details{8, true, true}},
            {"float",   Details{4, true, true}},
            {"double",  Details{8, true, true}},
            {"int",     Details{4, true}},
            {"unsigned",Details{4, false}},
            {"long",    Details{8, true}},
            {"byte",    Details{1, false}},
            {"char",    Details{1, true}},
            {"short",   Details{2, false}}
        };

        auto it = NumTypeDetails.find(tp);
        name = it->first;
        return it->second;
    }

    std::string NumberType::toString(bool compressed) const
    {
        return std::string{mName};
    }
}