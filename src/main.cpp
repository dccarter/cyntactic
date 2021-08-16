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
// simple import
import hello;
import std.Console; /* Console */
import std.{Console, Debug} -> Temp; /* referred to as Temp.Console or Temp.Debug */
)";
    Parser p;
    auto pg = p.parse(Source, "<stdin>");
    pg.dump(std::cout);
    return 0;
}
