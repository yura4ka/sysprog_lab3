#ifndef LAB3_LEXEMES_H
#define LAB3_LEXEMES_H

enum class LexemeType {
    Error,
    Identifier,
    Keyword,
    Operator,
    Comment,
    MultilineComment,
    Integer,
    Binary,
    Octal,
    Hexadecimal,
    Float,
    FloatHexadecimal,
    Rune,
    String,
    Eof,
};

class Lexemes {
private:
    static const std::map<LexemeType, std::string> LEXEMES;
    static const std::set<std::string> OPERATORS;
    static const std::set<std::string> KEYWORDS;
public:
    static std::string toString(LexemeType lexeme);

    static bool isOperator(const char &c);

    static bool isOperator(const std::string &s);

    static bool isIdentifier(const char &c);

    static bool isKeyword(const std::string &s);

    static bool isOctal(const char &c);

    static bool isHexadecimal(const char &c);

    static bool isEscapable(const char &c);

    static bool isQuote(const char &c);

    static bool isAllowOperator(const char &c);
};

#endif //LAB3_LEXEMES_H
