#pragma once

#include <variant>
#include <string>
#include <map>

enum class TokenType {
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
    Null,
    Any,
    _EOF
};

static const std::map<TokenType, const char*> TokenNamesMap = {
    {TokenType::Comma, "Comma"},
    {TokenType::String, "String"},
    {TokenType::Number, "Number"},
    {TokenType::ArrayEnd, "Number"},
    {TokenType::Comma, "Comma"},
    {TokenType::String, "String"},
    {TokenType::Number, "Number"},
    {TokenType::ArrayStart, "ArrayStart"},
    {TokenType::ArrayEnd, "ArrayEnd"},
    {TokenType::ObjectStart, "ObjectStart"},
    {TokenType::ObjectEnd, "ObjectEnd"},
    {TokenType::WhiteSpace, "WhiteSpace"},
    {TokenType::NewLine, "NewLine"},
    {TokenType::Tab, "Tab"},
    {TokenType::Column, "Column"},
    {TokenType::Bool, "Bool"},
    {TokenType::Null, "Null"},
    {TokenType::Any, "Any"},
    {TokenType::_EOF, "_EOF"}
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
        return TokenNamesMap.at(_type);
    }   
};

