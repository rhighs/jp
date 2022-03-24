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

std::string Tokenizer::parse_string(char stop_at) {
    std::string parsed_string;

    while (_current_char != stop_at) {
        if (_eof_reached) {
            parsing_error();
        }

        parsed_string += _current_char;
        advance();
    }

    return parsed_string;
}

double Tokenizer::parse_number() {
    std::string integer_string;

    while (is_digit(_current_char)) {
        if (_eof_reached) {
            parsing_error();
        }

        integer_string += _current_char;
        advance();
    }

    return std::stod(integer_string);
}

bool Tokenizer::parse_null() {
    std::string null_string;

    for (size_t i = 0; i < 4; ++i) {
        if (_eof_reached) {
            parsing_error();
        }

        null_string += _current_char;

        if (i < 3) {
            advance();
        }
    }

    return null_string == "null";
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

        if (i < 4) {
            advance();
        }
    }

    if (bool_string != "false") {
        parsing_error();
    }

    return false;
}

Token Tokenizer::next_token() {
    while (!_eof_reached) {
        if (is_digit(_current_char)) {
            return Token(TokenType::Number, parse_number());
        }

        Token t;
        switch (_current_char) {
            case '\'': 
            case '\"': {
                char stop_at = _current_char;
                advance();
                t = Token(TokenType::String, parse_string(stop_at));
                break;
            }
            case ',': {
                t = Token(TokenType::Comma, _current_char);
                break;
            }
            case '[': {
                t = Token(TokenType::ArrayStart, _current_char);
                break;
            }
            case ']': {
                t =  Token(TokenType::ArrayEnd, _current_char);
                break;
            }
            case '{': {
                t = Token(TokenType::ObjectStart, _current_char);
                break;
            }
            case '}': {
                t = Token(TokenType::ObjectEnd, _current_char);
                break;
            }
            case ':': {
                t = Token(TokenType::Column, _current_char);
                break;
            }
            case ' ': {
                t = Token(TokenType::WhiteSpace, _current_char);
                break;
            }
            case '\n': {
                t = Token(TokenType::NewLine, _current_char);
                break;
            }
            case '\t': {
                t = Token(TokenType::Tab, _current_char);
                break;
            }
            default: {
                if (can_be_bool()) {
                    t = Token(TokenType::Bool, parse_bool());
                } else if (can_be_null()) {
                    t = Token(TokenType::Null, parse_null());
                }

                break;
            }
        }

        advance();
        return t;
    }

    return Token(TokenType::_EOF, ' ');
}

bool Tokenizer::is_digit(char some_char) const {
    return (some_char >= 48/*0*/ && some_char <= 57/*9*/) || some_char == '.';
}

bool Tokenizer::can_be_bool() const {
    return _text.substr(_pos, 4) == "true"
        || _text.substr(_pos, 5) == "false";
}

bool Tokenizer::can_be_null() const {
    return _text.substr(_pos, 4) == "null";
}
