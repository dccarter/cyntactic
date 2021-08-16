//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <string>
#include <ostream>

#include <node.hpp>

namespace cyntactic {

    class Program : public Node {
    public:
        Program() : Node(Node::PROGRAM){};
        void dump(std::ostream& os) const;
    protected:
        std::string toString(bool compressed = true) const override;
    };
}
