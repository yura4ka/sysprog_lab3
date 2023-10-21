#ifndef LAB3_LEXER_H
#define LAB3_LEXER_H

#include <fstream>
#include <map>
#include <vector>
#include <set>
#include "lexemes.h"

enum class State {
    Start,
    Error,
    Identifier,
    Operator,
    Slash,
    Comment,
    MLComment,
    MLCommentEnd,
    Number_0,
    Number,
    Number_,
    NumberBinStart,
    NumberBin,
    NumberBin_,
    NumberOctStart,
    NumberOct,
    NumberOct_,
    NumberHexStart,
    NumberHex,
    NumberHex_,
    ZeroNumber,
    ZeroNumber_,
    MustFloat,
    MustFloat_,
    Dot,
    Float,
    Float_,
    FloatExp,
    FloatExpSign,
    FloatExpFull,
    FloatHexShort,
    FloatHex,
    FloatHex_,
    FloatHexExp,
    FloatHexSign,
    FloatHexFull,
    Rune,
    RuneEscape,
    RuneFull,
    String,
    StringEscape,
    RawString,
};

struct Token {
    LexemeType type;
    std::string literal;
};

class Lexer {
private:
    std::ifstream _file;
    char _c;
    State _state;

    void readChar();

    Token nextLexeme();

public:
    explicit Lexer(const std::string &filename);

    std::vector<Token> getLexemes();
};


#endif //LAB3_LEXER_H
