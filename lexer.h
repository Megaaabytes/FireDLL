#ifndef FIREDLL_LEXER_H
#define FIREDLL_LEXER_H
#pragma once
#include <vector>
#include <iostream>
#include <map>
#include "token.h"

using std::vector;
using std::string;

class Lexer {
    bool noCharacters = false;
    string src;
    unsigned long position;
    char current;
    size_t line;
    std::vector<Token*> tokens;

    void Advance();
    Token* ParseID();
    Token* ParseDigit();
    Token* ParseNegativeDigit();
    Token* ParseString();
    Token* ParseWString();
    Token* IdentifyToken(char token);
    char Peek();
public:
    vector<Token*>& GetTokens();

    Lexer& operator=(Lexer& right) = delete;

    ~Lexer() {
        for(Token* t : tokens) {
            delete t;
        }
    }

    explicit Lexer(string src) {
        if(src.empty()) noCharacters = true;

        this->src = src;
        this->position = 0;
        this->line = 1;
        this->current = src[position];
    }
};

#endif //FIREDLL_LEXER_H
