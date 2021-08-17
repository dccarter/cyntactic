//
// Created by Mpho Mbotho on 2021-08-13.
//

#include "exceptions.hpp"
#include "ast/binexpr.hpp"
#include "ast/import.hpp"
#include "ast/identifier.hpp"
#include "ast/literal.hpp"
#include "symbols.hpp"

#include "parser.hpp"

namespace {
    using cyntactic::Node;
    using cyntactic::Program;
}

namespace cyntactic {

    Program Parser::parse(const std::string_view& code, const std::string_view& src)
    {
        mTokenizer.reset(code, src);
        Program pg;
        advance();
        while (!is(Token::T_EOF))
        {
            eatWhiteSpace();
            switch (mLookahead.kind) {
                case Token::IMPORT: {
                    pg.Children.push_back(importExpr());
                    break;
                }
                case Token::COMMENT:
                    advance(true); // ignore all comments
                    break;
                default: {
                    pg.Children.push_back(binaryExpr());
                    expectAdvance("expression's missing terminal semi-colon ';'", Token::SEMICOLON);
                    break;
                }
            }
            eatWhiteSpace();
        }

        return std::move(pg);
    }

    template<typename ...Args>
    void Parser::syntaxError(Args&... args)
    {
        throw SyntaxError(
                mTokenizer.source(),
                mTokenizer.line(),
                mTokenizer.column(),
                std::forward<Args>(args)...);
    }

    template <typename... T>
    bool Parser::expectCheck(Token::Kind kind, T&&... kinds)
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

    template<typename... T>
    void Parser::expectAdvance(const std::string_view& msg, Token::Kind kind, T&&... kinds)
    {
        expect(msg, kind, std::forward<T>(kinds)...);
        advance();
    }

    template<typename... T>
    void Parser::expect(const std::string_view& msg, Token::Kind kind, T&&... kinds)
    {
        if (!expectCheck(kind, std::forward<T>(kinds)...)) {
            syntaxError(msg);
        }
    }

    template<typename T, typename... Args>
    Node::Ptr Parser::mkNode(Args&&... args)
    {
        Node::Ptr node = std::make_unique<T>(std::forward<Args>(args)...);
        node->Source = mLookahead.Source;
        node->Line = mLookahead.Line;
        node->Column = mLookahead.Column;
        return std::move(node);
    }

    void Parser::commaSeperatedIdentifier(TokenFunc onIdent)
    {
        auto consumeIdentifier = [&] {
            eatWhiteSpace();
            expect("unexpected token, expecting identifier", Token::IDENTIFIER);
            onIdent(mLookahead);
            advance(true);
        };

        consumeIdentifier();
        while (is(Token::COMMA))
        {
            advance();
            consumeIdentifier();
        }
    }

    void Parser::advance(bool eatWs)
    {
        mLookahead = mTokenizer.next();
        if (eatWs) eatWhiteSpace();
    }

    Node::Ptr Parser::advance(Node::Ptr &&node, bool eatWs)
    {
        advance(eatWs);
        return std::move(node);
    }

    void Parser::eatWhiteSpace()
    {
        while (is(Token::WHITESPACE))
            advance();
    }

    Node::Ptr Parser::integerLiteral(int base)
    {
        auto str = std::string{mLookahead.Value};
        return advance(
                mkNode<ast::Literal>(std::stoull(str, nullptr, base)),
                true);
    }

    Node::Ptr Parser::charLiteral()
    {
        return advance(
                mkNode<ast::Literal>(mLookahead.Value[0]),
                true);
    }

    Node::Ptr Parser::stringLiteral()
    {
        return advance(
                mkNode<ast::Literal>(std::string{mLookahead.Value}),
                true);
    }

    Node::Ptr Parser::boolLiteral()
    {
        return advance(
                mkNode<ast::Literal>(mLookahead.Value == "true"),
                true);
    }

    Node::Ptr Parser::importExpr()
    {
        expectAdvance("unexpected token, expecting 'import'", Token::IMPORT);
        expectAdvance("'import' keyword should be followed by 1 or more spaces", Token::WHITESPACE);
        expect("invalid import statement, expecting name of module", Token::IDENTIFIER);

        auto node = std::make_unique<ast::Import>();
        node->Name = std::string{mLookahead.Value};
        advance();

        if (is(Token::DOT)) {
            advance();
            if (is(Token::IDENTIFIER)) {
                node->Symbols.emplace_back(mLookahead.Value);
                advance();
            }
            else {
                expectAdvance("unexpected token, expecting '{' or symbol name",Token::LBRACE);
                commaSeperatedIdentifier([&](const Token& tok) {
                    node->Symbols.emplace_back(tok.Value);
                });
                expectAdvance("unexpected token, expecting '}' to import symbols", Token::RBRACE);
            }
        }

        eatWhiteSpace();
        if (is(Token::RARROW)) {
            advance(true);
            expect("unexpected token, expecting the name of the symbol ", Token::IDENTIFIER);
            node->Alias = mLookahead.Value;
            advance(true);
        }
        expectAdvance("import statement must be terminated by a ';'", Token::SEMICOLON);
        return std::move(node);
    }

    Node::Ptr Parser::primaryExpr()
    {
        switch (mLookahead.kind) {
            case Token::IDENTIFIER: {
                if (!SymTable::isDefined(mLookahead.Value)) {
                    syntaxError("variable '", mLookahead.Value, "' not defined");
                }
                return advance(mkNode<ast::Identifier>(mLookahead.Value), true);
            }
            case Token::HEX_LITERAL:
                return integerLiteral(16);
            case Token::BIN_LITERAL:
                return integerLiteral(2);
            case Token::OCT_LITERAL:
                return integerLiteral(8);
            case Token::DEC_LITERAL:
                return integerLiteral(10);
            case Token::CHAR_LITERAL:
                return charLiteral();
            case Token::BOOL_LITERAL:
                return boolLiteral();
            case Token::STRING:
                return stringLiteral();
            default:
                syntaxError("unexpected token, expecting primary-expression");
        }

        return nullptr;
    }

    Node::Ptr Parser::binaryExpr(unsigned int precedence)
    {
        auto getOperator = [&]() {
            auto op = ast::BinaryOpInfo::find(mLookahead.kind);
            if (!op) {
                syntaxError("unexpected token, expecting binary operator");
            }
            return op;
        };

        Node::Ptr left{nullptr}, right{nullptr};
        left = primaryExpr();
        if (is(Token::SEMICOLON) || is(Token::T_EOF)) {
            return std::move(left);
        }

        auto op = getOperator();

        while (op.Precedence > precedence) {
            advance(true);

            right = binaryExpr(op.Precedence);
            left  = mkNode<ast::BinaryExpr>(
                        op, std::move(left), std::move(right));

            if (is(Token::SEMICOLON) || is(Token::T_EOF)) {
                return std::move(left);
            }

            op = getOperator();
        }

        return std::move(left);
    }
}