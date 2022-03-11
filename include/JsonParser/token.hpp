#pragma once

#include <variant>
#include <string>

enum TokenType {
    Comma,
    String,
    Number,
    ArrayStart,
    ArrayEnd,
    ObjectStart,
    ObjectEnd,
    WhiteSpace,
    NewLine,
    Tab,
    Column,
    Bool,
    Any,
    _EOF
};

using TokenValue = std::variant<std::string, double, char, bool>;

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

    std::string name() const {
        switch (_type) {
            case Comma:
                return "Comma";
            case String:
                return "String";
            case Number:
                return "Number";
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
            case _EOF:
                return "EOF";
            case NewLine:
                return "NewLne";
            case Tab:
                return "Tab";
            default:
                return "Any";
        }
    }   
};

