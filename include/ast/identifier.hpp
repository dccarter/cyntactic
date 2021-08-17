//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <node.hpp>

namespace cyntactic::ast {

    class Identifier : public Node {
    public:
        Identifier() : Node(Node::IDENT) {}
        Identifier(const std::string_view& name)
            : Node(Node::IDENT), Name{name}
        {}
        std::string Name{};
    protected:
        std::string toString(bool compressed = true) const override;
    };
}
