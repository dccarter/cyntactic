//
// Created by Mpho Mbotho on 2021-08-13.
//

#pragma once

#include <string_view>
#include <tuple>
#include <unordered_map>

namespace cyntactic {

struct Token {
    typedef enum {
        T_EOF,
        BOOL_LITERAL,           // true|false
        CHAR_LITERAL,           // Any character under single quotes
        INT_LITERAL,            // Hex|Bin|Oct|Dec
        FLOAT_LITERAL,          // Any floating point
        IDENTIFIER,             // valid c++ identifier
        WHITESPACE,             // any consecutive white space
        STRING,                 // valid C-string
        COMMENT,                // Valid c++ comment
        STREXPR,                // Start of a string expression ${
        LBRACE,                 // {
        RBRACE,                 // }
        LPAREN,                 // (
        RPAREN,                 // )
        LBRACKET,               // [
        RBRACKET,               // ]
        PLUS,                   // +
        MINUS,                  // -
        STAR,                   // *
        SLASH,                  // /
        COMPTIME,               // @
        DOT,                    // .
        SEMICOLON,              // ;
        AMPERSAND,              // &
        BAR,                    // |
        COLON,                  // :
        GREATER_THAN,           // >
        LESS_THAN,              // <
        EQUALS,                 // =
        COMMA,                  // ,
        PERCENT,                // %
        QUESTION,               // ?
        CARET,                  // ^
        GRAVE,                  // `
        TILDE,                  // ~
        EXCLAMATION,            // !
        RARROW,                 // ->
        LARROW,                 // <-
        OP_EQ,                  // ==
        OP_NEQ,                 // !=
        OP_GTE,                 // >=
        OP_LTE,                 // <=
        OP_PLUS_EQ,             // +=
        OP_MINUS_EQ,            // -=
        OP_MULT_EQ,             // *=
        OP_DIV_EQ,              // /=
        OP_MOD_EQ,              // %=
        OP_AND_EQ,              // &=
        OP_OR_EQ,               // |=
        OP_XOR_EQ,              // ^=
        OP_LSHIFT,              // <<
        OP_RSHIFT,              // >>
        OP_LAND,                // &&
        OP_LOR,                 // ||
        OP_PLUS_PLUS,           // ++
        OP_MINUS_MINUS,         // --
        OP_IS,                  // is
        OP_IN,                  // in
        OP_SCOPE,               // ::
        OP_SEQUENCE,            // ..
        OP_ELIPSE,              // ...
        OP_LSHIFT_EQ,           // <<=
        OP_RSHIFT_EQ,           // >>=
        IF,
        IN,
        AS,
        FOR,
        AUTO,
        CASE,
        ELSE,
        FROM,
        FUNC,
        NONE,
        VOID,
        THIS,
        BREAK,
        DEFER,
        RAISE,
        WHILE,
        ASYNC,
        AWAIT,
        USING,
        IMPORT,
        MODULE,
        NATIVE,
        RETURN,
        STRUCT,
        SIZEOF,
        SWITCH,
        CONTINUE,
        INT_TYPE,
        STR_TYPE,
        BOOL_TYPE,
        CODE_TYPE,
        FLOAT_TYPE
    } Kind;
    Kind kind{T_EOF};
    std::string_view Value{};
    void toString(std::ostream& os, bool includeValue = true) const;
};

static const std::unordered_map<std::string_view, Token::Kind> Keywords{
    {"if",          Token::IF},
    {"in",          Token::IN},
    {"as",          Token::AS},
    {"for",         Token::FOR},
    {"auto",        Token::AUTO},
    {"case",        Token::CASE},
    {"break",        Token::BREAK},
    {"defer",        Token::DEFER},
    {"raise",        Token::RAISE},
    {"else",        Token::ELSE},
    {"from",        Token::FROM},
    {"func",        Token::FUNC},
    {"void",        Token::VOID},
    {"null",        Token::NONE},
    {"func",        Token::FUNC},
    {"this",        Token::THIS},
    {"using",       Token::USING},
    {"async",       Token::ASYNC},
    {"await",       Token::AWAIT},
    {"while",       Token::WHILE},
    {"module",      Token::MODULE},
    {"import",      Token::IMPORT},
    {"native",      Token::NATIVE},
    {"switch",      Token::SWITCH},
    {"return",      Token::RETURN},
    {"sizeof",      Token::SIZEOF},
    {"struct",      Token::STRUCT},
    {"continue",    Token::CONTINUE},
    {"bool",        Token::BOOL_TYPE},
    {"short",       Token::INT_TYPE},
    {"ushort",      Token::INT_TYPE},
    {"int",         Token::INT_TYPE},
    {"uint",        Token::INT_TYPE},
    {"long",        Token::INT_TYPE},
    {"ulong",       Token::INT_TYPE},
    {"byte",        Token::INT_TYPE},
    {"char",        Token::INT_TYPE},
    {"i8",          Token::INT_TYPE},
    {"u8",          Token::INT_TYPE},
    {"i16",         Token::INT_TYPE},
    {"u16",         Token::INT_TYPE},
    {"i32",         Token::INT_TYPE},
    {"u32",         Token::INT_TYPE},
    {"i64",         Token::INT_TYPE},
    {"u64",         Token::INT_TYPE},
    {"f32",         Token::FLOAT_TYPE},
    {"f64",         Token::FLOAT_TYPE},
    {"string",      Token::STR_TYPE},
    {"code",        Token::CODE_TYPE}
};

template <typename T, T T1, T... Ts>
struct any_of {
    bool operator()(const T& c) {
        if (c == T1) return true;
        if constexpr(sizeof...(Ts) > 0) {
            return any_of<T, Ts...>{}(c);
        }
        return false;
    }
};

class Tokenizer {
public:
    Tokenizer(std::string_view code, const std::string_view& src = "<stdin>")
        : mCode{code},
          mSource{src}
    {}

    Tokenizer() = default;

    void reset(std::string_view code, const std::string_view& src = "<stdin>");
    const std::string_view& source() const { return mSource; }
    std::size_t line() const { return mLine; }
    std::size_t column() const { return mCol; }

    Token next();

private cynt_ut:
    char peek() const;
    std::pair<char, char> peekTwo() const;
    std::tuple<char, char, char> peekThree() const;

    Token tok(Token&& tok, unsigned c = 1);
    void eat(unsigned c = 1);
    void eatWhiteSpace();

    Token parseString();
    Token parseHexFloat(std::size_t start) { return {}; }
    Token parseHexNumber();
    Token parseCharacter();
    Token parseIdentifier();
    Token parseOctalNumber();
    Token parseBinaryNumber();
    Token parseDecimalFloat(std::size_t start) { return {}; }
    Token parseDecimalNumber();
    Token parseMultiLineComment();
    Token parseSingleLineComment();

    std::string_view mCode{};
    std::string_view mSource{};
    std::size_t  mPos{0};
    std::size_t mLine{1};
    std::size_t mCol{1};
};

}