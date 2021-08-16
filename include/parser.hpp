//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <ast.hpp>
#include <tokenizer.hpp>

#include <parser.hpp>

#include <filesystem>
#include <functional>

namespace cyntactic {

    /**
     * Grammar notation
     * _ [\s\r\t\n]
     * __ [\s\t]
     */

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
        ast::Program parse(const std::filesystem::path& src);

        /**
         * Parses the code contained in the given \p code string
         * @param code the code to parse
         * @param src the source file from which the code come from
         * @return the parsed source syntax tree
         */
        ast::Program parse(const std::string_view& code, const std::string_view& src);

    private:
        ast::Node::Ptr importExpr();
        ast::Node::Ptr integerLiteral();

    private:
        using TokenFunc = std::function<void(const Token&)>;

        void advance() { mLookahead = mTokenizer.next(); }
        bool is(Token::Kind kind) const { return mLookahead.kind == kind; }
        void eatWhiteSpace();
        void commaSeperatedIdentifier(TokenFunc onIdent);

        template<typename ...Args>
        void syntaxError(Args&... args);

        template<typename... T>
        void expect(const std::string_view& msg, Token::Kind kind, T&&... kinds)
        {
            if (!expectCheck(kind, std::forward<T>(kinds)...)) {
                syntaxError(msg);
            }
        }

        template<typename... T>
        void expectAdvance(const std::string_view& msg, Token::Kind kind, T&&... kinds)
        {
            expect(msg, kind, std::forward<T>(kinds)...);
            advance();
        }

        template<typename... T>
        bool expectCheck(Token::Kind kind, T&&... kinds)
        {
            if (mLookahead.kind == kind) {
                return true;
            }
            if constexpr(sizeof...(T)) {
                if (expect(std::forward<T>(kinds)...)) {
                    return true;
                }
            }
            return false;
        }

        Tokenizer mTokenizer;
        Token mLookahead{};
    };
}