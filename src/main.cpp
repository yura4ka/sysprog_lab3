#include <iostream>
#include "lexer.h"
#include "lexemes.h"

int main() {
    Lexer lexer("../input.go");
    auto result = lexer.getLexemes();
    for (const Token &r: result) {
        std::cout << r.literal << " " << Lexemes::toString(r.type) << "\n";
    }
    return 0;
}
