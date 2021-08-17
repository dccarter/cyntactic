#include "parser.hpp"
#include "trie.hpp"

#include <string>
#include <iostream>

using cyntactic::Node;
using cyntactic::Token;
using cyntactic::Parser;

int main(int argc, char *argv[])
{
    const std::string Source =
R"(
4 * 5 / 2 + 3;
6 + one;
)";
    Parser p;
    auto pg = p.parse(Source, "<stdin>");
    pg.dump(std::cout);
    return 0;
}
