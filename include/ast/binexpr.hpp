//
// Created by Mpho Mbotho on 2021-08-16.
//

#pragma once

#include <node.hpp>
#include <tokenizer.hpp>

namespace cyntactic::ast {

    enum class BinaryOp {
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
    };

    struct BinaryOpInfo {
        BinaryOp Op{BinaryOp::OP_NONE};
        std::string_view Str{};
        unsigned Precedence{0};
        operator bool() const { return Op != BinaryOp::OP_NONE; }
        static BinaryOpInfo find(Token::Kind token);
    };

    class BinaryExpr : public Node {
    public:
        BinaryExpr() : Node(Node::BINARY_EXPR) {}
        BinaryExpr(BinaryOpInfo op, Node::Ptr&& left, Node::Ptr&& right)
            : Node(Node::BINARY_EXPR),
              Op{op}
        {
            Children.push_back(std::move(left));
            Children.push_back(std::move(right));
        }
        BinaryOpInfo Op{};

    protected:
        std::string toString(bool compressed = true) const override;
    };
}
