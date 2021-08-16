//
// Created by Mpho Mbotho on 2021-08-13.
//

#include "ast.hpp"
#include "trie.hpp"

namespace cyntactic {

    template <>
    std::pair<ast::Node::iterator , ast::Node::iterator> TreeGraph<ast::Node>::countChildren() const {
        return std::make_pair(mNode.Children.begin(), mNode.Children.end());
    }

    template <>
    bool TreeGraph<ast::Node>::isOneliner() const { return !mNode.Children.empty(); }

    template <>
    const ast::Node& TreeGraph<ast::Node>::getNode(const Iterator& it) const {
        return *(*it);
    }

    template <>
    std::string TreeGraph<ast::Node>::createAtom() const { return mNode.toString(); }
}

namespace cyntactic::ast {

    NumberType::NumberType(const std::string_view &tp)
        : mDetails{getDetails(tp, mName)}
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

    std::string Program::toString(bool compressed) const
    {
        return "Program";
    }

    std::string astImport::toString(bool compressed) const
    {
        std::stringstream ss;
        ss << "Import (" << Name;
        if (!Symbols.empty()) {
            if (!compressed) {
                ss << "/{";
                for (const auto& sym: Symbols) {
                    if (&sym != &Symbols[0]) ss << ", ";
                    ss << sym;
                }
                ss << "}";
            }
            else {
                ss << "{...}";
            }
        }
        if (!Identity.empty()) {
            ss << " as " << Identity;
        }
        ss << ")";
        return ss.str();
    }
}