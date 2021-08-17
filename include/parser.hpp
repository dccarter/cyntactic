//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <filesystem>
#include <functional>

#include <program.hpp>
#include <tokenizer.hpp>
#include <parser.hpp>

namespace cyntactic {

    class Parser {
    public:
        Parser() = default;
        /**
         * Parses the source code from the give source file
         * @param src the source file to parse
         * @return the parsed source code tree
         *
         * Program :
         *  IntegerLiteral
         */
         Program parse(const std::filesystem::path& src);

        /**
         * Parses the code contained in the given \p code string
         * @param code the code to parse
         * @param src the source file from which the code come from
         * @return the parsed source syntax tree
         */
        Program parse(const std::string_view& code, const std::string_view& src);

    private:
        Node::Ptr importExpr();
        Node::Ptr primaryExpr();
        Node::Ptr binaryExpr(unsigned precedence = 0);
        Node::Ptr integerLiteral(int base);
        Node::Ptr charLiteral();
        Node::Ptr stringLiteral();
        Node::Ptr boolLiteral();

    private:
        using TokenFunc = std::function<void(const Token&)>;

        void advance(bool eatWs = false);
        Node::Ptr advance(Node::Ptr&& node, bool eatWs = false);
        bool is(Token::Kind kind) const { return mLookahead.kind == kind; }
        void eatWhiteSpace();
        void commaSeperatedIdentifier(TokenFunc onIdent);

        template<typename ...Args>
        void syntaxError(Args&... args);
        template<typename... T>
        void expect(const std::string_view& msg, Token::Kind kind, T&&... kinds);
        template<typename... T>
        void expectAdvance(const std::string_view& msg, Token::Kind kind, T&&... kinds);
        template<typename... T>
        bool expectCheck(Token::Kind kind, T&&... kinds);

        template<typename T, typename... Args>
        Node::Ptr mkNode(Args&&... args);

        Tokenizer mTokenizer;
        Token mLookahead{};
    };
}