#include <string>
#include <iostream>

#include "JsonParser/tokenizer.hpp"

void Tokenizer::parsing_error() const {
    std::cerr << "Invalid token error at: " << _pos << std::endl;
    exit(1);
}

void Tokenizer::advance() {
    _pos++;
    if (_pos >= _text.size()) {
        _eof_reached = true;
    } else {
        _current_char = _text.at(_pos);
    }
}

void Tokenizer::consume_whitespaces() {
    while (_current_char == ' ') {
        advance();
    }
}

std::string Tokenizer::parse_string() {
    std::string parsed_string;

    while (!is_string_delimiter(_current_char)) {
        if (_eof_reached) {
            parsing_error();
        }

        parsed_string += _current_char;
        advance();
    }

    return parsed_string;
}

int Tokenizer::parse_integer() {
    std::string integer_string;

    while (is_digit(_current_char)) {
        if (_eof_reached) {
            parsing_error();
        }

        integer_string += _current_char;
        advance();
    }

    return std::stoi(integer_string);
}

bool Tokenizer::parse_bool() {
    std::string bool_string;

    for (size_t i = 0; i < 5; ++i) {
        if (_eof_reached) {
            parsing_error();
        }

        bool_string += _current_char;

        if (bool_string == "true") {
            return true;
        }

        advance();
    }

    if (bool_string != "false") {
        parsing_error();
    }

    return false;
}

Token Tokenizer::next_token() {
    while (!_eof_reached) {
        if (is_digit(_current_char)) {
            return Token(Integer, parse_integer());
        }

        Token t;
        switch (_current_char) {
            case '\'': 
            case '\"': {
                advance();
                t = Token(String, parse_string());
                break;
            }
            case ',': {
                t = Token(Comma, _current_char);
                break;
            }
            case '[': {
                t = Token(ArrayStart, _current_char);
                break;
            }
            case ']': {
                t =  Token(ArrayEnd, _current_char);
                break;
            }
            case '{': {
                t = Token(ObjectStart, _current_char);
                break;
            }
            case '}': {
                t = Token(ObjectEnd, _current_char);
                break;
            }
            case ':': {
                t = Token(Column, _current_char);
                break;
            }
            case ' ': {
                t = Token(WhiteSpace, _current_char);
                break;
            }
            default: {
                if (can_be_bool()) {
                    t = Token(Bool, parse_bool());
                } else {
                    t = Token(Any, _current_char);
                }

                break;
            }
        }

        advance();
        return t;
    }

    return Token(_EOF, ' ');
}

bool Tokenizer::is_digit(char some_char) const {
    return some_char >= 48 && some_char <= 57;
}

bool Tokenizer::is_string_delimiter(char some_char) const {
    return some_char == '\'' || some_char == '\"';
}

bool Tokenizer::can_be_bool() const {
    return _text.substr(_pos, 4) == "true"
        || _text.substr(_pos, 5) == "false";
}
