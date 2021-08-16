//
// Created by Mpho Mbotho on 2021-08-16.
//

#include "ast/import.hpp"
#include "parser.hpp"

namespace cyntactic::ast {

    std::string Import::toString(bool compressed) const
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
        if (!Alias.empty()) {
            ss << " as " << Alias;
        }
        ss << ")";
        return ss.str();
    }


}