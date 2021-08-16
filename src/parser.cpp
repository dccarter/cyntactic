//
// Created by Mpho Mbotho on 2021-08-13.
//

#include "exceptions.hpp"
#include "ast/import.hpp"

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
                    pg.Children.emplace_back(importExpr());
                    break;
                }
                case Token::COMMENT:
                    advance(); // ignore all comments
                    break;
                default: {
                    syntaxError("unexpected token");
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

    void Parser::eatWhiteSpace()
    {
        while (is(Token::WHITESPACE))
            advance();
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
}