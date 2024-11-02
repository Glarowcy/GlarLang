#include "Parser.hh"
#include <iostream>

char const* src = R"(
    let var: int = 5;
)";

int main(void) {
    Parser parser;

    parser.initParser(src);
    parser.generateAst();
    parser.printAst();
}