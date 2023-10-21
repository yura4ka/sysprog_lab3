#include <map>
#include <set>
#include <string>
#include "lexemes.h"

const std::map<LexemeType, std::string> Lexemes::LEXEMES = {
        {LexemeType::Error,            "ERROR"},
        {LexemeType::Identifier,       "IDENTIFIER"},
        {LexemeType::Keyword,          "KEYWORD"},
        {LexemeType::Operator,         "OPERATOR"},
        {LexemeType::Comment,          "COMMENT"},
        {LexemeType::MultilineComment, "MULTILINE_COMMENT"},
        {LexemeType::Integer,          "INTEGER"},
        {LexemeType::Binary,           "BINARY"},
        {LexemeType::Octal,            "OCTAL"},
        {LexemeType::Hexadecimal,      "HEXADECIMAL"},
        {LexemeType::Float,            "FLOAT"},
        {LexemeType::FloatHexadecimal, "FLOAT_HEXADECIMAL"},
        {LexemeType::Rune,             "RUNE"},
        {LexemeType::String,           "STRING"},
        {LexemeType::Eof,              "EOF"},

};

const std::set<std::string> Lexemes::OPERATORS = {
        "+", "&", "+=", "&=", "&&", "==", "!=", "(", ")", "-", "|", "-=", "|=", "||", "<", "<=", "[", "]", "*", "^",
        "*=", "^=", "<-", ">", ">=", "{", "}", "/", "<<", "/=", "<<=", "++", "=", ":=", ",", ";", "%", ">>", "%=",
        ">>=", "--", "!", "...", ".", ":", "&^", "&^=", "~"
};

const std::set<std::string> Lexemes::KEYWORDS = {
        "break", "default", "func", "interface", "select", "case", "defer",
        "go", "map", "struct", "chan", "else", "goto", "package", "switch",
        "const", "fallthrough", "if", "range", "type", "continue", "for",
        "import", "return", "var", "uint8", "uint16", "uint32", "uint64", "int8", "int16", "int32", "int64", "float32",
        "float64", "complex64", "complex128", "byte", "rune", "uint", "int", "intptr"
};

std::string Lexemes::toString(LexemeType lexeme) {
    return LEXEMES.at(lexeme);
}

bool Lexemes::isOperator(const std::string &s) {
    return OPERATORS.count(s) == 1;
}

bool Lexemes::isOperator(const char &c) {
    std::string s({c});
    return isOperator(s);
}

bool Lexemes::isIdentifier(const char &c) {
    return c == '_' || std::isalnum(c);
}

bool Lexemes::isKeyword(const std::string &s) {
    return KEYWORDS.count(s) == 1;
}

bool Lexemes::isOctal(const char &c) {
    return c >= '0' && c <= '7';
}

bool Lexemes::isHexadecimal(const char &c) {
    return std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Lexemes::isEscapable(const char &c) {
    const char esc[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '\"'};

    for (char i: esc)
        if (i == c)
            return true;
    return false;
}

bool Lexemes::isQuote(const char &c) {
    return c == '\'' || c == '"' || c == '`';
}

bool Lexemes::isAllowOperator(const char &c) {
    return std::isspace(c) || isQuote(c) || isIdentifier(c) || isOperator(c);
}
