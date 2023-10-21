#include <vector>
#include "lexer.h"
#include "lexemes.h"

Lexer::Lexer(const std::string &filename) {
    _file = std::ifstream(filename);
    if (!_file.is_open())
        throw std::runtime_error("File doesn't exists!");
    _state = State::Start;
    _c = '\0';
    readChar();
}

void Lexer::readChar() {
    if (!_file.get(_c))
        _c = EOF;
}

Token Lexer::nextLexeme() {
    std::string lexeme;
    _state = State::Start;
    while (true) {
        switch (_state) {
            case State::Start:
                if (std::isspace(_c))
                    break;
                if (_c == EOF)
                    return {LexemeType::Eof, lexeme};
                if (_c == '.')
                    _state = State::Dot;
                else if (_c == '/')
                    _state = State::Slash;
                else if (std::isdigit(_c))
                    _state = _c != '0' ? State::Number : State::Number_0;
                else if (_c == '\'')
                    _state = State::Rune;
                else if (_c == '\"')
                    _state = State::String;
                else if (_c == '`')
                    _state = State::RawString;
                else if (Lexemes::isIdentifier(_c))
                    _state = State::Identifier;
                else if (Lexemes::isOperator(_c))
                    _state = State::Operator;
                else
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::Error:
                if (std::isspace(_c) || Lexemes::isOperator(_c) || _c == EOF)
                    return {LexemeType::Error, lexeme};
                lexeme += _c;
                break;
            case State::Identifier:
                if (Lexemes::isIdentifier(_c)) {
                    lexeme += _c;
                    break;
                }
                if (Lexemes::isKeyword(lexeme))
                    return {LexemeType::Keyword, lexeme};
                else
                    return {LexemeType::Identifier, lexeme};
            case State::Operator:
                if (Lexemes::isOperator(lexeme + _c)) {
                    lexeme += _c;
                    break;
                }
                if (!Lexemes::isAllowOperator(_c) && _c != EOF) {
                    _state = State::Error;
                    break;
                }
                return {LexemeType::Operator, lexeme};
            case State::Slash:
                if (_c == '/')
                    _state = State::Comment;
                else if (_c == '*')
                    _state = State::MLComment;
                else if (Lexemes::isOperator(lexeme + _c))
                    _state = State::Operator;
                else if (!std::isspace(_c) && !Lexemes::isIdentifier(_c) && !Lexemes::isOperator(_c) && _c != EOF)
                    _state = State::Error;
                else
                    return {LexemeType::Operator, lexeme};
                lexeme += _c;
                break;
            case State::Comment:
                if (_c == '\n' || _c == EOF)
                    return {LexemeType::Comment, lexeme};
                lexeme += _c;
                break;
            case State::MLComment:
                if (_c == EOF)
                    return {LexemeType::Error, lexeme};
                if (_c == '*')
                    _state = State::MLCommentEnd;
                lexeme += _c;
                break;
            case State::MLCommentEnd:
                if (_c == EOF)
                    return {LexemeType::Error, lexeme};
                lexeme += _c;
                if (_c == '/') {
                    readChar();
                    return {LexemeType::MultilineComment, lexeme};
                }
                _state = State::MLComment;
                break;
            case State::Number:
                if (_c == '_')
                    _state = State::Number_;
                else if (_c == '.' || _c == 'e' || _c == 'E')
                    _state = State::Float;
                else if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Integer, lexeme};
                else if (!std::isdigit(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::Number_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = std::isdigit(_c) ? State::Number : State::Error;
                lexeme += _c;
                break;
            case State::Number_0:
                if (_c == 'b' || _c == 'B')
                    _state = State::NumberBinStart;
                else if (_c == 'o' || _c == 'O')
                    _state = State::NumberOctStart;
                else if (_c == '_')
                    _state = State::NumberOct_;
                else if (_c == 'x' || _c == 'X')
                    _state = State::NumberHexStart;
                else if (_c == '.' || _c == 'e' || _c == 'E')
                    _state = State::Float;
                else if (Lexemes::isOctal(_c))
                    _state = State::ZeroNumber;
                else if (_c == '8' || _c == '9')
                    _state = State::MustFloat;
                else if (std::isspace(_c) || Lexemes::isOperator(_c))
                    return {LexemeType::Integer, lexeme};
                else
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::NumberBinStart:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                if (_c == '_')
                    _state = State::NumberBin_;
                else
                    _state = _c == '0' || _c == '1' ? State::NumberOct : State::Error;
                lexeme += _c;
                break;
            case State::NumberBin:
                if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Binary, lexeme};
                if (_c == '_')
                    _state = State::NumberBin_;
                else if (!(_c == '0' || _c == '1'))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::NumberBin_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = _c == '0' || _c == '1' ? State::NumberBin : State::Error;
                lexeme += _c;
                break;
            case State::NumberOctStart:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                if (_c == '_')
                    _state = State::NumberOct_;
                else
                    _state = Lexemes::isOctal(_c) ? State::NumberOct : State::Error;
                lexeme += _c;
                break;
            case State::NumberOct:
                if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Octal, lexeme};
                if (_c == '_')
                    _state = State::NumberOct_;
                else if (!Lexemes::isOctal(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::NumberOct_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = Lexemes::isOctal(_c) ? State::NumberOct : State::Error;
                lexeme += _c;
                break;
            case State::NumberHexStart:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                if (_c == '_')
                    _state = State::NumberHex_;
                else if (_c == '.')
                    _state = State::FloatHexShort;
                else
                    _state = Lexemes::isHexadecimal(_c) ? State::NumberHex : State::Error;
                lexeme += _c;
                break;
            case State::NumberHex:
                if (_c == '_')
                    _state = State::NumberHex_;
                else if (_c == '.')
                    _state = State::FloatHex;
                else if (_c == 'p' || _c == 'P')
                    _state = State::FloatHexExp;
                else if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Hexadecimal, lexeme};
                else if (!Lexemes::isHexadecimal(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::NumberHex_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = Lexemes::isHexadecimal(_c) ? State::NumberHex : State::Error;
                lexeme += _c;
                break;
            case State::ZeroNumber:
                if (_c == '_')
                    _state = State::ZeroNumber_;
                else if (_c == '.')
                    _state = State::Float;
                else if (_c == '8' || _c == '9')
                    _state = State::MustFloat;
                else if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Octal, lexeme};
                else if (!std::isdigit(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::ZeroNumber_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                if (_c == '8' || _c == '9')
                    _state = State::MustFloat;
                else if (!std::isdigit(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::MustFloat:
                if (_c == '.')
                    _state = State::Float;
                else if (_c == '_')
                    _state = State::MustFloat_;
                else if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Error, lexeme};
                else if (!std::isdigit(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::MustFloat_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = std::isdigit(_c) ? State::MustFloat : State::Error;
                lexeme += _c;
                break;
            case State::Dot:
                if (Lexemes::isOperator(lexeme + _c))
                    _state = State::Operator;
                else if (_c == '.') {
                    char next = (char) _file.peek();
                    if (next == '.') {
                        readChar();
                        readChar();
                        return {LexemeType::Operator, "..."};
                    }
                    return {LexemeType::Operator, lexeme};
                } else if (std::isdigit(_c))
                    _state = State::Float;
                else if (Lexemes::isAllowOperator(_c))
                    return {LexemeType::Operator, lexeme};
                else
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::Float:
                if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Float, lexeme};
                if (_c == 'e' || _c == 'E')
                    _state = State::FloatExp;
                else if (_c == '_' && lexeme[lexeme.size() - 1] != '.')
                    _state = State::Float_;
                else if (!std::isdigit(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::Float_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = std::isdigit(_c) ? State::Float : State::Error;
                lexeme += _c;
                break;
            case State::FloatExp:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                if (std::isdigit(_c))
                    _state = State::FloatExpFull;
                else if (_c == '-' || _c == '+')
                    _state = State::FloatExpSign;
                else
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::FloatExpSign:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = std::isdigit(_c) ? State::FloatExpFull : State::Error;
                lexeme += _c;
                break;
            case State::FloatExpFull:
                if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::Float, lexeme};
                if (_c == '_')
                    _state = State::FloatExpSign;
                else if (!std::isdigit(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::FloatHexShort:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = Lexemes::isHexadecimal(_c) ? State::FloatHex : State::Error;
                lexeme += _c;
                break;
            case State::FloatHex:
                if (_c == 'p' || _c == 'P')
                    _state = State::FloatHexExp;
                else if (_c == '_' && lexeme[lexeme.size() - 1] != '.')
                    _state = State::FloatHex_;
                else if (!Lexemes::isHexadecimal(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::FloatHex_:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = Lexemes::isHexadecimal(_c) ? State::FloatHex : State::Error;
                lexeme += _c;
                break;
            case State::FloatHexExp:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                if (Lexemes::isHexadecimal(_c))
                    _state = State::FloatHexFull;
                else if (_c == '-' || _c == '+')
                    _state = State::FloatHexSign;
                else
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::FloatHexSign:
                if (std::isspace(_c))
                    return {LexemeType::Error, lexeme};
                _state = Lexemes::isHexadecimal(_c) ? State::FloatHexFull : State::Error;
                lexeme += _c;
                break;
            case State::FloatHexFull:
                if (std::isspace(_c) || Lexemes::isOperator({_c}))
                    return {LexemeType::FloatHexadecimal, lexeme};
                if (_c == '_')
                    _state = State::FloatHexSign;
                else if (!Lexemes::isHexadecimal(_c))
                    _state = State::Error;
                lexeme += _c;
                break;
            case State::Rune:
                if (_c == EOF)
                    return {LexemeType::Error, lexeme};
                lexeme += _c;
                if (_c == '\'') {
                    readChar();
                    return {LexemeType::Error, lexeme};
                }
                if (_c == '\\')
                    _state = State::RuneEscape;
                else
                    _state = State::RuneFull;
                break;
            case State::RuneEscape:
                if (!Lexemes::isEscapable(_c))
                    _state = State::Error;
                else
                    _state = State::RuneFull;
                lexeme += _c;
                break;
            case State::RuneFull:
                lexeme += _c;
                if (_c == '\'') {
                    readChar();
                    return {LexemeType::Rune, lexeme};
                }
                _state = State::Error;
                break;
            case State::String:
                if (_c == EOF)
                    return {LexemeType::Error, lexeme};
                lexeme += _c;
                if (_c == '\"') {
                    readChar();
                    return {LexemeType::String, lexeme};
                }
                if (_c == '\\')
                    _state = State::StringEscape;
                break;
            case State::StringEscape:
                if (!Lexemes::isEscapable(_c))
                    _state = State::Error;
                else
                    _state = State::String;
                lexeme += _c;
                break;
            case State::RawString:
                if (_c == EOF)
                    return {LexemeType::Error, lexeme};
                lexeme += _c;
                if (_c == '`') {
                    readChar();
                    return {LexemeType::String, lexeme};
                }
                break;
            default:
                lexeme += _c;
                _state = State::Error;
                break;
        }
        readChar();
    }
}

std::vector<Token> Lexer::getLexemes() {
    std::vector<Token> result;
    while (!_file.eof()) {
        result.emplace_back(nextLexeme());
    }
    return result;
}
