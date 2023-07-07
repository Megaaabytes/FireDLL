#ifndef FIREDLL_TOKEN_H
#define FIREDLL_TOKEN_H


struct Token {
    enum TokenType {
        NULLTOKEN,
        SPACE,
        NEWLINE,
        STRING,
        WSTRING,
        DIGIT,
        NEGATIVE_DIGIT,
        TOKEN_FLOAT,
        NEGATIVE_FLOAT,
        ID,
        NULLPTR,
        TOKEN_EOF,
    };

    TokenType tokenType;
    std::string data;

    Token(TokenType type, std::string data) {
        this->tokenType = type;
        this->data = std::move(data);
    }
};

#endif //FIREDLL_TOKEN_H
