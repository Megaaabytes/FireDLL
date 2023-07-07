#include "lexer.h"
#include <sstream>

using std::exception;
using std::stringstream;

void Lexer::Advance() {
    if(position == src.length() - 1)
        current = '\0';
    else {
        position++;
        current = src[position];
    }
}

Token* Lexer::ParseID() {
    string result;

    while(isalnum(current)) {
        result += current;
        Advance();
    }

    if(result == "nullptr")
        return new Token(Token::NULLPTR, result);
    else if(result == "wstr") {
        if(Peek() == '"') {
            Advance();
            Advance();
            Token* str = ParseString();
            Advance();
            str->tokenType = Token::WSTRING;
            return str;
        }
        else if(isalpha(Peek())) {
            Advance();
            Token* str = ParseID();
            Advance();
            str->tokenType = Token::WSTRING;
            return str;
        }
    }

    return new Token(Token::TokenType::ID, result);
}

Token* Lexer::ParseDigit() {
    string result;
    bool isDecimal = false;

    while(isdigit(current) || current == '.') {
        if(current == '.') {
            if(isDecimal) {
                throw std::exception("Float number cannot have more than one decimal point.");
            }
            else isDecimal = true;
        }

        result += current;
        Advance();
    }

    return new Token(isDecimal ? Token::TOKEN_FLOAT : Token::DIGIT, result);
}

Token* Lexer::ParseNegativeDigit() {
    Token* digit = ParseDigit();

    if(digit->tokenType == Token::TOKEN_FLOAT)
        digit->tokenType = Token::NEGATIVE_FLOAT;
    else if(digit->tokenType == Token::DIGIT)
        digit->tokenType = Token::NEGATIVE_DIGIT;

    return digit;
}

Token* Lexer::ParseString() {
    string result;

    while(current != '"') {
        if(current == '\0')
            throw std::exception("Unexpected EOF in string.");

        if(current == '\\') {
            Advance();
            switch(current) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 'b': result += '\b'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                case '0': result += '\0'; break;
                case 'a': result += '\a'; break;
                case 'v': result += '\v'; break;
                case 'f': result += '\f'; break;
                case 'x': {
                    Advance();
                    char first = current;
                    Advance();
                    char second = current;

                    char hex[2];
                    hex[0] = first;
                    hex[1] = second;

                    int hexAscii = strtol(hex, nullptr, 16);
                    char append = (char)hexAscii;
                    result += append;
                    break;
                }
                default: {
                    stringstream s;
                    s << "Unrecognized escape sequence: \'\\";
                    s << current;
                    s << '\'';
                    s << " at line: ";
                    s << line;
                    s << '.';
                    s << '\n';

                    throw exception(s.str().c_str());
                }
            }

            Advance();
            continue;
        }

        result += current;
        Advance();
    }

    return new Token(Token::STRING, result);
}

Token* Lexer::ParseWString() {
    Token* str = ParseString();
    str->tokenType = Token::WSTRING;

    return str;
}

Token* Lexer::IdentifyToken(char token) {
    switch(token) {
        case (char)32: return new Token(Token::TokenType::SPACE, "CHAR 32");
        case (char)9: return new Token(Token::TokenType::SPACE, "CHAR 9");
        case '\0': return new Token(Token::TokenType::NULLTOKEN, "");
        case '\n': {
            line++;
            return new Token(Token::TokenType::NEWLINE, "NEWLINE");
        }
        case '\r': return new Token(Token::TokenType::SPACE, "\r");
        default: {
            stringstream s;
            s << "Unrecognized token: \'";
            s << token;
            s << '\'';
            s << " at line: ";
            s << line;
            s << '.';
            s << '\n';

            throw exception(s.str().c_str());
        }
    }
}

char Lexer::Peek() {
    if(position == src.length() - 1)
        return '\0';
    else
        return src[position + 1];
}

vector<Token*>& Lexer::GetTokens() {
    while(current != '\0') {
        if(noCharacters)
            break;

        if(current == '"') {
            Advance();
            try {
                Token* token = ParseString();
                tokens.push_back(token);
            }
            catch (const std::exception& ex) {
                std::cout << ex.what() << '\n';
                for(Token* t : tokens) {
                    delete t;
                }

                exit(1);
            }

            goto advance;
        }

        if(isalpha(current)) {
            try {
                Token* id = ParseID();
                tokens.push_back(id);
            } catch (const std::exception& ex) {
                std::cout << ex.what() << '\n';
                for(Token* t : tokens) {
                    delete t;
                }

                exit(1);
            }
        }

        if(isdigit(current)) {
            try {
                Token* token = ParseDigit();
                tokens.push_back(token);
            }
            catch (const std::exception& ex) {
                std::cout << ex.what() << '\n';
                for(Token* t : tokens) {
                    delete t;
                }

                exit(1);
            }
        }

        if(current == '-') {
            Advance();
            try {
                Token* token = ParseNegativeDigit();
                tokens.push_back(token);
            }
            catch (const std::exception& ex) {
                std::cout << ex.what() << '\n';
                for(Token* t : tokens) {
                    delete t;
                }

                exit(1);
            }
        }

        try {
            Token* nextToken = IdentifyToken(current);

            if(nextToken->tokenType == Token::NULLTOKEN) {
                delete nextToken;
                break;
            } else if(nextToken->tokenType == Token::NEWLINE) {
                delete nextToken;
                goto advance;
            } else if(nextToken->tokenType == Token::SPACE) {
                delete nextToken;
                goto advance;
            } else tokens.push_back(nextToken);
        }
        catch (const std::exception& ex) {
            std::cout << ex.what() << '\n';
            for(Token* t : tokens) {
                delete t;
            }

            exit(1);
        }

        advance:
        Advance();
    }

    return tokens;
}