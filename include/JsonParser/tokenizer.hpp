#pragma once

#include "JsonParser/token.hpp"

class Tokenizer {
    std::string _text;
    char _current_char;
    bool _eof_reached;
    size_t _pos;

    bool can_be_bool() const;
    bool can_be_null() const;
    void parsing_error() const;
    bool is_digit(char some_char) const;
    bool is_string_delimiter(char some_char) const;

    void advance();
    bool parse_null();
    bool parse_bool();
    double parse_number();
    std::string parse_string(char stop_at);
    void consume_whitespaces();

public:
    Tokenizer(std::string text)
        : _text(text), _pos(0), _eof_reached(false), _current_char(_text.at(0)) {}

    Token next_token();

    size_t pos() const {
        return _pos;
    }

    const std::string& text() const {
        return _text;
    }
};
