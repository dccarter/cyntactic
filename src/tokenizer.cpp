#include "tokenizer.hpp"
#include "exceptions.hpp"

#include <vector>
#include <utility>

namespace {
    std::string_view charString(char c)
    {
        static char buf[10];
        if (c == EOF) {
            return "EOF";
        }
        if (!isprint(c)) {
            size_t count = sprintf(buf, "ascii-%02X", c);
            return {buf, count};
        }
        else {
            size_t count = sprintf(buf, "%c", c);
            return {buf, count};
        }
    }

    inline bool ishex(char c)
    {
        return (c >= '0' and c <= '9') ||
               (c >= 'a' and c <= 'f') ||
               (c >= 'A' and c <= 'F');
    }
}

namespace cyntactic {

using escapable = any_of<char, 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '"', '?', '0'>;
std::string_view escaped(char c)
{
    const std::unordered_map<char, std::string_view> ESCAPABLE = {
        {'b',  "\b"},
        {'f',  "\f"},
        {'n',  "\n"},
        {'r',  "\r"},
        {'t',  "\t"},
        {'v',  "\v"},
        {'\\', "\\"},
        {'\'', "\'"},
        {'"',  "\""},
        {'?',  "\?"},
        {'0',  "\0"}
    };
    auto it = ESCAPABLE.find(c);
    if (it != ESCAPABLE.end()) {
        return it->second;
    }
    return {};
}

void Tokenizer::reset(std::string_view code, const std::string_view& src)
{
    mSource = src;
    mCode = code;
    mLine = 1;
    mPos = 0;
    mCol = 1;
}

std::tuple<char, char, char> Tokenizer::peekThree() const
{
    auto i = mPos;
    auto ii = i + 1;
    auto iii = ii + 1;

    return {
        ((i   < mCode.size())? mCode[i]:  char(EOF)),
        ((ii  < mCode.size())? mCode[ii]:  char(EOF)),
        ((iii < mCode.size())? mCode[iii]: char(EOF))
    };
}

std::pair<char, char> Tokenizer::peekTwo() const
{
    auto i  = mPos;
    auto ii = i + 1;
    return {
        ((i   < mCode.size())? mCode[i]:  char(EOF)),
        ((ii  < mCode.size())? mCode[ii]:  char(EOF))
    };
}

char Tokenizer::peek() const
{
    return (mPos < mCode.size())? mCode[mPos] : char(EOF);
}

Token Tokenizer::tok(Token&& tok, unsigned c)
{
    eat(c);
    return tok;
}

void Tokenizer::eat(unsigned c)
{
    for (auto i = 0; i < c; i++, mPos++) {
        if (mPos >= mCode.size()) break;
        if (mCode[mPos] == '\n') {
            mLine++;
            mCol = 0;
        }
        else {
            mCol++;
        }
    }
}

void Tokenizer::eatWhiteSpace()
{
    while (isspace(peek()))
        eat();
}

Token Tokenizer::parseCharacter()
{
    auto [c, cc, ccc] = peekThree();
    auto val = mCode.substr(mPos, 1);

    if (c == '\\') {
        eat(); // eat the escape
        if (escapable{}(cc)) {
            val = escaped(cc);
            cc = ccc;
        }
        else {
            throw SyntaxError(mSource, mLine, mCol,
                    "character escape '\\", charString(cc), "' is not supported");
        }
    }

    if (cc != '\'') {
        eat(); // eat the character
        throw SyntaxError(mSource, mLine, mCol, "unexpected character '", charString(cc), "', expecting a \"'\"");
    }
    return tok({Token::CHAR_LITERAL, val}, 2);
}

Token Tokenizer::parseIdentifier()
{
    auto start = mPos;
    eat();
    do {
        auto c = peek();
        if (isalnum(c) || c == '_') {
            eat();
        }
        else {
            break;
        }
    } while(true);

    auto var = mCode.substr(start, (mPos - start));
    auto it = Keywords.find(var);
    if (it != Keywords.end()) {
        return {it->second, it->first};
    }
    return {Token::IDENTIFIER, var};
}

Token Tokenizer::parseString()
{
    auto start = mPos;
    do {
        auto [c, cc] = peekTwo();
        if (c == '\\') {
            if (escapable{}(cc) || cc == '$') {
                eat(2);
            }
            else {
                throw SyntaxError(mSource, mLine, mCol,
                          "unexpected escaped character, '", charString(cc), "'");
            }
        }
        else if (c == '$' and cc == '{') {
            // return whatever we have so far, it's part of a string expression
            return {Token::STRING, mCode.substr(start, mPos-start)};
        }
        else if (c == '"') {
            // end of string found
            break;
        }
        else {
            eat();
        }
    } while (true);

    return tok({Token::STRING, mCode.substr(start, mPos-start)});
}

Token Tokenizer::parseHexNumber()
{
    auto start = mPos;
    eat(2);
    do {
        auto c = peek();
        if (ishex(c)) {
            eat();
        }
        else {
            break;
        }
    } while(true);

    auto c = peek();
    if (any_of<char, '.','p','P'>{}(c)) {
        return parseHexFloat(start);
    }

    return tok({Token::INT_LITERAL, mCode.substr(start, mPos-start)});
}

Token Tokenizer::parseOctalNumber()
{
    auto start = mPos;
    eat();
    do {
        auto c = peek();
        if (c >= '0' && c <= '7') {
            eat();
        }
        else {
            break;
        }
    } while(true);

    return tok({Token::INT_LITERAL, mCode.substr(start, mPos-start)});
}

Token Tokenizer::parseBinaryNumber()
{
    auto start = mPos;
    eat(2);
    do {
        auto c = peek();
        if (c == '0' || c == '1') {
            eat();
        }
        else {
            break;
        }
    } while(true);

    return tok({Token::INT_LITERAL, mCode.substr(start, mPos-start)});
}

Token Tokenizer::parseDecimalNumber()
{
    auto start = mPos;
    eat();
    do {
        auto c = peek();
        if (isdigit(c)) {
            eat();
        }
        else {
            break;
        }
    } while(true);

    auto c = peek();
    if (any_of<char, '.', 'e', 'E'>{}(c)) {
        return parseDecimalFloat(start);
    }

    return tok({Token::INT_LITERAL, mCode.substr(start, mPos-start)});
}

Token Tokenizer::parseMultiLineComment()
{
    auto start = mPos;
    do {
        auto [c, cc] = peekTwo();
        if (c == EOF) {
            throw SyntaxError(mSource, mLine, mCol,
                      "unterminated multiline comment, EOF before */");
        }

        if (c  == '*' and cc == '/') {
            break;
        }
        else {
            eat();
        }
    } while (true);

    return tok({Token::COMMENT, mCode.substr(start, mPos-start)}, 2);
}

Token Tokenizer::parseSingleLineComment()
{
    auto start = mPos;
    while(peek() != '\n') eat();
    return tok({Token::COMMENT, mCode.substr(start, mPos-start)});
}


Token Tokenizer::next()
{
    auto [c, cc, ccc] = peekThree();
    if (c == EOF) {
        return {};
    }

    switch(c) {
        case '@': return tok({Token::COMPTIME});
        case '{': return tok({Token::LBRACE});
        case '}': return tok({Token::RBRACE});
        case '(': return tok({Token::LPAREN});
        case ')': return tok({Token::RPAREN});
        case '[': return tok({Token::LBRACKET});
        case ']': return tok({Token::RBRACKET});
        case ',': return tok({Token::COMMA});
        case ';': return tok({Token::SEMICOLON});
        case '?': return tok({Token::QUESTION});
        case '`': return tok({Token::GRAVE});
        case '=': {
            if (cc ==  '=') {
                return tok({Token::OP_EQ}, 2);
            }
            else {
                return tok({Token::OP_EQ});
            }
        }
        case '!': {
            if (cc ==  '=') {
                return tok({Token::OP_NEQ}, 2);
            }
            else {
                return tok({Token::EXCLAMATION});
            }
        }
        case '+': {
            switch(cc) {
                case '+': return tok({Token::OP_PLUS_PLUS},2);
                case '=': return tok({Token::OP_PLUS_EQ}, 2);
                default: return tok({Token::PLUS});
            }
            break;
        }
        case '-': {
            switch(cc) {
                case '>': return tok({Token::RARROW},2);
                case '-': return tok({Token::OP_MINUS_MINUS},2);
                case '=': return tok({Token::OP_MINUS_EQ}, 2);
                default: return tok({Token::MINUS});
            }
            break;
        }
        case '*': {
            switch(cc) {
                case '=': return tok({Token::OP_MULT_EQ}, 2);
                default: return tok({Token::STAR});
            }
            break;
        }
        case '/': {
            switch(cc) {
                case '=': return tok({Token::OP_DIV_EQ}, 2);
                case '/': { eat(2); return parseSingleLineComment(); }
                case '*': { eat(2); return parseMultiLineComment(); }
                default: return tok({Token::SLASH});
            }
            break;
        }
        case '%': {
            switch(cc) {
                case '=': return tok({Token::OP_MOD_EQ}, 2);
                default: return tok({Token::PERCENT});
            }
            break;
        }
        case '^': {
            switch(cc) {
                case '=': return tok({Token::OP_XOR_EQ}, 2);
                default: return tok({Token::CARET});
            }
            break;
        }
        case '|': {
            switch(cc) {
                case '|': return tok({Token::OP_LOR},2);
                case '=': return tok({Token::OP_OR_EQ}, 2);
                default: return tok({Token::BAR});
            }
            break;
        }
        case '&': {
            switch(cc) {
                case '&': return tok({Token::OP_LAND},2);
                case '=': return tok({Token::OP_AND_EQ}, 2);
                default: return tok({Token::AMPERSAND});
            }
            break;
        }
        case '.': {
            if (cc ==  '.') {
                if (ccc == '.') {
                    return tok({Token::OP_ELIPSE}, 3);
                }
                return tok({Token::OP_SEQUENCE}, 2);
            }
            return tok({Token::DOT});
        }
        case ':': {
            if (cc ==  ':') {
                return tok({Token::OP_SCOPE}, 2);
            }
            return tok({Token::COLON});
        }
        case '<': {
            switch(cc) {
                case '=': return tok({Token::OP_LTE},2);
                case '-': return tok({Token::LARROW}, 2);
                case '<': {
                    if (ccc == '=') {
                        return tok({Token::OP_LSHIFT_EQ}, 3);
                    }
                    return tok({Token::OP_LSHIFT}, 2);
                }
                default: return tok({Token::LESS_THAN});
            }
            break;
        }
        case '>': {
            switch(cc) {
                case '=': return tok({Token::OP_GTE},2);
                case '>': {
                    if (ccc == '=') {
                        return tok({Token::OP_RSHIFT_EQ}, 3);
                    }
                    return tok({Token::OP_RSHIFT}, 2);
                }
                default: return tok({Token::GREATER_THAN});
            }
            break;
        }
        case '"': { eat(); return parseString(); }
        case '\'': { eat(); return parseCharacter(); }
        case '0': {
            switch(cc) {
                case 'b':
                case 'B': { return parseBinaryNumber(); }
                case 'x':
                case 'X': { return parseHexNumber(); }
                default: break;
            }

            if (isdigit(cc)) {
                return parseOctalNumber();
            }
            else {
                // a decimal digit which is 0
                return tok({Token::INT_LITERAL, mCode.substr(mPos, 1)});
            }
        }
        default: {
            if (isspace(c)) {
                eat();
                eatWhiteSpace();
                return {Token::WHITESPACE};
            }
            else if (isdigit(c)) {
                return parseDecimalNumber();
            }
            else if (isalpha(c) || c == '_'){
                return parseIdentifier();
            }
            else if (c == '$' and cc == '{') {
                return tok({Token::STREXPR}, 2);
            }
            else {
                eat();
                throw SyntaxError(
                    mSource, mLine, mCol,
                    "Unexpected '", charString(c), "'");
            }
        }
    }

    return {Token::T_EOF};
}

void Token::toString(std::ostream& os, bool includeValue) const
{
    switch (kind) {
        case Token::T_EOF: { os << "T_EOF"; break; }
        case Token::BOOL_LITERAL: { os << "BOOL_LITERAL"; break; }
        case Token::CHAR_LITERAL: { os << "CHAR_LITERAL"; break; }
        case Token::INT_LITERAL: { os << "INT_LITERAL"; break; }
        case Token::FLOAT_LITERAL: { os << "FLOAT_LITERAL"; break; }
        case Token::IDENTIFIER: { os << "IDENTIFIER"; break; }
        case Token::WHITESPACE: { os << "WHITESPACE"; break; }
        case Token::STRING: { os << "STRING"; break; }
        case Token::COMMENT: { os << "COMMENT"; break; }
        case Token::STREXPR: { os << "STREXPR"; break; }
        case Token::LBRACE: { os << "LBRACE"; break; }
        case Token::RBRACE: { os << "RBRACE"; break; }
        case Token::LPAREN: { os << "LPAREN"; break; }
        case Token::RPAREN: { os << "RPAREN"; break; }
        case Token::LBRACKET: { os << "LBRACKET"; break; }
        case Token::RBRACKET: { os << "RBRACKET"; break; }
        case Token::PLUS: { os << "PLUS"; break; }
        case Token::MINUS: { os << "MINUS"; break; }
        case Token::STAR: { os << "STAR"; break; }
        case Token::SLASH: { os << "SLASH"; break; }
        case Token::COMPTIME: { os << "COMPTIME"; break; }
        case Token::DOT: { os << "DOT"; break; }
        case Token::SEMICOLON: { os << "SEMICOLON"; break; }
        case Token::AMPERSAND: { os << "AMPERSAND"; break; }
        case Token::BAR: { os << "BAR"; break; }
        case Token::COLON: { os << "COLON"; break; }
        case Token::GREATER_THAN: { os << "GREATER_THAN"; break; }
        case Token::LESS_THAN: { os << "LESS_THAN"; break; }
        case Token::EQUALS: { os << "EQUALS"; break; }
        case Token::COMMA: { os << "COMMA"; break; }
        case Token::PERCENT: { os << "PERCENT"; break; }
        case Token::QUESTION: { os << "QUESTION"; break; }
        case Token::CARET: { os << "CARET"; break; }
        case Token::GRAVE: { os << "GRAVE"; break; }
        case Token::TILDE: { os << "TILDE"; break; }
        case Token::EXCLAMATION: { os << "EXCLAMATION"; break; }
        case Token::RARROW: { os << "RARROW"; break; }
        case Token::LARROW: { os << "LARROW"; break; }
        case Token::OP_EQ: { os << "OP_EQ"; break; }
        case Token::OP_NEQ: { os << "OP_NEQ"; break; }
        case Token::OP_GTE: { os << "OP_GTE"; break; }
        case Token::OP_LTE: { os << "OP_LTE"; break; }
        case Token::OP_PLUS_EQ: { os << "OP_PLUS_EQ"; break; }
        case Token::OP_MINUS_EQ: { os << "OP_MINUS_EQ"; break; }
        case Token::OP_MULT_EQ: { os << "OP_MULT_EQ"; break; }
        case Token::OP_DIV_EQ: { os << "OP_DIV_EQ"; break; }
        case Token::OP_MOD_EQ: { os << "OP_MOD_EQ"; break; }
        case Token::OP_AND_EQ: { os << "OP_AND_EQ"; break; }
        case Token::OP_OR_EQ: { os << "OP_OR_EQ"; break; }
        case Token::OP_XOR_EQ: { os << "OP_XOR_EQ"; break; }
        case Token::OP_LSHIFT: { os << "OP_LSHIFT"; break; }
        case Token::OP_RSHIFT: { os << "OP_RSHIFT"; break; }
        case Token::OP_LAND: { os << "OP_LAND"; break; }
        case Token::OP_LOR: { os << "OP_LOR"; break; }
        case Token::OP_PLUS_PLUS: { os << "OP_PLUS_PLUS"; break; }
        case Token::OP_MINUS_MINUS: { os << "OP_MINUS_MINUS"; break; }
        case Token::OP_IS: { os << "OP_IS"; break; }
        case Token::OP_IN: { os << "OP_IN"; break; }
        case Token::OP_SCOPE: { os << "OP_SCOPE"; break; }
        case Token::OP_SEQUENCE: { os << "OP_SEQUENCE"; break; }
        case Token::OP_ELIPSE: { os << "OP_ELIPSE"; break; }
        case Token::OP_LSHIFT_EQ: { os << "OP_LSHIFT_EQ"; break; }
        case Token::OP_RSHIFT_EQ: { os << "OP_RSHIFT_EQ"; break; }
        case Token::IF: { os << "IF"; break; }
        case Token::IN: { os << "IN"; break; }
        case Token::AS: { os << "AS"; break; }
        case Token::FOR: { os << "FOR"; break; }
        case Token::AUTO: { os << "AUTO"; break; }
        case Token::CASE: { os << "CASE"; break; }
        case Token::ELSE: { os << "ELSE"; break; }
        case Token::FROM: { os << "FROM"; break; }
        case Token::FUNC: { os << "FUNC"; break; }
        case Token::NONE: { os << "NONE"; break; }
        case Token::VOID: { os << "VOID"; break; }
        case Token::THIS: { os << "THIS"; break; }
        case Token::BREAK: { os << "BREAK"; break; }
        case Token::DEFER: { os << "DEFER"; break; }
        case Token::RAISE: { os << "RAISE"; break; }
        case Token::WHILE: { os << "WHILE"; break; }
        case Token::ASYNC: { os << "ASYNC"; break; }
        case Token::AWAIT: { os << "AWAIT"; break; }
        case Token::USING: { os << "USING"; break; }
        case Token::IMPORT: { os << "IMPORT"; break; }
        case Token::MODULE: { os << "MODULE"; break; }
        case Token::NATIVE: { os << "NATIVE"; break; }
        case Token::RETURN: { os << "RETURN"; break; }
        case Token::STRUCT: { os << "STRUCT"; break; }
        case Token::SIZEOF: { os << "SIZEOF"; break; }
        case Token::SWITCH: { os << "SWITCH"; break; }
        case Token::CONTINUE: { os << "CONTINUE"; break; }
        case Token::INT_TYPE: { os << "INT_TYPE"; break; }
        case Token::STR_TYPE: { os << "STR_TYPE"; break; }
        case Token::BOOL_TYPE: { os << "BOOL_TYPE"; break; }
        case Token::CODE_TYPE: { os << "CODE_TYPE"; break; }
        case Token::FLOAT_TYPE: { os << "FLOAT_TYP"; break; }
        default: { os << "Tok_" << int(kind); break; }
    }

    if (includeValue and !Value.empty()) {
        os << "(" << Value << ")";
    }
    os << "\n";
}
}

#ifdef SYNTATIC_UNITTEST
#include <catch2/catch.hpp>

using cyntactic::Token;
using cyntactic::Tokenizer;
#endif