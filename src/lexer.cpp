#include "lexer.h"
#include "shell.h"
#include <iostream>
#include <string>

std::unordered_map<std::string, Token::Type> keywords = {
    {"cd", Token::Type::CD},
};

std::vector<Token> Lexer::ScanTokens() {
    while (!IsAtEnd()) {
        start_ = current_;
        ScanToken();
    }
    return tokens_;
}

void Lexer::ScanToken() {
    char c = Advance();
    switch (c) {
    case '|':
        /* code */
        AddToken(Token::Type::PIPE);
        break;
    case '<':
        AddToken(Token::Type::REDIRECT_IN);
        break;
    case '>':
        // Add redirect append
        if (Match('>')) {
            AddToken(Token::Type::REDIRECT_APPEND);
        } else {
            AddToken(Token::Type::REDIRECT_OUT);
        }
        break;
    case '&':
        AddToken(Token::Type::BACKGROUND);
        break;
    case '"':
        String();
        break;
    case ' ':
        // Ignore whitespace
        break;
    case '\t':
        // Ignore whitespace
        break;
    default:
        Word();
        break;
    }
}

char Lexer::Advance() { return source_[current_++]; }

char Lexer::Peek() {
    if (IsAtEnd()) {
        return '\0';
    }
    return source_[current_];
}

bool Lexer::Match(char expected) {
    if (IsAtEnd()) {
        return false;
    }
    if (source_[current_] != expected) {
        return false;
    }
    current_++;
    return true;
}

bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' ||
           c == '.' || c == '-' || c == '/' || c == '~';
}

bool IsAlphaNumeric(char c) { return IsAlpha(c) || isdigit(c); }

void Lexer::Word() {
    // Whitespaced delimted
    while (!IsAtEnd() && IsAlphaNumeric(Peek())) {
        Advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    try {
        Token::Type type = keywords.at(text);
        AddToken(type);
    } catch (const std::out_of_range &e) {
        AddToken(Token::Type::WORD);
        // Handle the exception
    }
}

void Lexer::String() {
    while (!IsAtEnd() && Peek() != '"') {
        Advance();
    }
    if (IsAtEnd()) {
        Error("unterminated string");
        return;
    }

    // The closing ".
    Advance();

    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    AddToken(Token::Type::WORD, value);
}

void Lexer::AddToken(Token::Type type, std::string value) {
    tokens_.push_back(Token(type, value));
}

void Lexer::AddToken(Token::Type type) {
    std::string text = source_.substr(start_, current_ - start_);
    tokens_.push_back(Token(type, text));
}
