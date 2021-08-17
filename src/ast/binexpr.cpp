//
// Created by Mpho Mbotho on 2021-08-16.
//
#include <unordered_map>

#include "ast/binexpr.hpp"

namespace cyntactic::ast {

    BinaryOpInfo BinaryOpInfo::find(Token::Kind token)
    {
        std::unordered_map<Token::Kind, BinaryOpInfo> BINARY_OPS = {
            {Token::PLUS, {BinaryOp::OP_ADD, "+", 10}},
            {Token::MINUS, {BinaryOp::OP_SUB, "-", 10}},
            {Token::STAR, {BinaryOp::OP_MUL, "*", 20}},
            {Token::SLASH, {BinaryOp::OP_DIV, "/", 20}},
            {Token::OP_EQ, {BinaryOp::OP_EQ, "==", 30}},
            {Token::OP_NEQ, {BinaryOp::OP_NEQ, "!=", 30}},
            {Token::LESS_THAN, {BinaryOp::OP_LT, "<", 40}},
            {Token::GREATER_THAN, {BinaryOp::OP_GT, ">", 40}},
            {Token::OP_LTE, {BinaryOp::OP_LEQ, "<=", 40}},
            {Token::OP_GTE, {BinaryOp::OP_GEQ, ">=", 40}}
        };

        auto it = BINARY_OPS.find(token);
        if (it == BINARY_OPS.end()) {
            return {};
        }
        return it->second;
    }

    std::string BinaryExpr::toString(bool compressed) const
    {
        return std::string{Op.Str};
    }

}