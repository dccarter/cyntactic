#include "parser.hpp"
#include "trie.hpp"

#include <string>
#include <iostream>

using cyntactic::ast::Node;
using cyntactic::Token;
using cyntactic::Parser;
using cyntactic::TreeGraph;
using cyntactic::TextBox;

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
    TreeGraph<Node> graph(pg, 132-2);
    TextBox dump{};
    dump.putbox(2, 0, graph());
    std::cout << dump.toString() << "\n";
    return 0;
}
