#include "tokenizer.hpp"
#include "trie.hpp"

#include <string>
#include <iostream>

using cyntactic::Token;
using cyntactic::Trie;

int main(int argc, char *argv[])
{
    struct About { std::uint8_t Size; bool Signed; bool Float; };
    Trie<About> trie {
        {"",        About{0, false, false}},
        {"i8",      About{1, true}},
        {"u8",      About{1, false}},
        {"i16",     About{2, true}},
        {"u16",     About{2, false}},
        {"i32",     About{4, true}},
        {"u32",     About{4, false}},
        {"i64",     About{8, true}},
        {"u64",     About{8, false}},
        {"f32",     About{4, true, true}},
        {"f64",     About{8, true, true}},
        {"float",   About{4, true, true}},
        {"double",  About{8, true, true}},
        {"int",     About{4, true}},
        {"unsigned",About{4, false}},
        {"long",    About{8, true}},
        {"byte",    About{1, false}},
        {"char",    About{1, true}},
        {"short",   About{2, false}}
    };

    trie.enumerate([](const std::string& key, const About& val) -> bool {
        std::cout << key << ": " << int(val.Size) << "\n";
        return true;
    }, "car");
}
