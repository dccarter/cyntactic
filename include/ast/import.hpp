//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <node.hpp>

namespace cyntactic {
    class Parser;
}

namespace cyntactic::ast {

    class Import : public Node {
    public:
        Import() : Node(Node::IMPORT){}
        std::string Name{};
        std::string Alias{};
        std::vector<std::string> Symbols{};

    protected:
        std::string toString(bool compressed = true) const override;
    };
}
