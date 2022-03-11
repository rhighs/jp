#pragma once

#include <variant>
#include <string>

enum TokenType {
    Comma,
    String,
    Integer,
    ArrayStart,
    ArrayEnd,
    ObjectStart,
    ObjectEnd,
    WhiteSpace,
    Column,
    Bool,
    Any,
    _EOF
};

using TokenValue = std::variant<std::string, int, char, bool>;

class Token {
    TokenType _type;
    TokenValue _value;

public:
    Token() = default;

    Token(TokenType type, TokenValue value)
        : _type(type), _value(value) {}

    TokenType type() const {
        return _type;
    }

    TokenValue value() const {
        return _value;
    }

    std::string name() {
        switch (_type) {
            case Comma:
                return "Comma";
            case String:
                return "String";
            case Integer:
                return "Integer";
            case ArrayStart:
                return "ArrayStart";
            case ArrayEnd:
                return "ArrayEnd";
            case ObjectStart:
                return "ObjectStart";
            case ObjectEnd:
                return "ObjectEnd";
            case WhiteSpace:
                return "WhiteSpace";
            case Column:
                return "Column";
            case Bool:
                return "Bool";
            default:
                return "Any";
        }
    }   
};

