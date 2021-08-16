//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <node.hpp>

namespace cyntactic::ast {

    class BinaryOp : Node {
    public:
        typedef enum {
            OP_NONE,
            OP_ADD,
            OP_SUB,
            OP_MUL,
            OP_DIV,
            OP_MOD,
            OP_NEQ,
            OP_LEQ,
            OP_GEQ,
            OP_EQ,
            OP_LT,
            OP_GT,
        } Operator;

        BinaryOp()
            : Node(Node::BINARY_OP)
        {}

        Operator Op{OP_NONE};
        unsigned Precedence{0};

    };

    class BinaryExpr : public Node {
    public:
        BinaryExpr() : Node(Node::BINARY_EXPR) {}
    };
}
