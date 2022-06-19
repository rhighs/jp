#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <cassert>
#include <iostream>
#include <optional>

namespace jp {

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
    {TokenType::Comma,          "Comma"},
    {TokenType::String,         "String"},
    {TokenType::Number,         "Number"},
    {TokenType::ArrayEnd,       "Number"},
    {TokenType::Comma,          "Comma"},
    {TokenType::String,         "String"},
    {TokenType::Number,         "Number"},
    {TokenType::ArrayStart,     "ArrayStart"},
    {TokenType::ArrayEnd,       "ArrayEnd"},
    {TokenType::ObjectStart,    "ObjectStart"},
    {TokenType::ObjectEnd,      "ObjectEnd"},
    {TokenType::WhiteSpace,     "WhiteSpace"},
    {TokenType::NewLine,        "NewLine"},
    {TokenType::Tab,            "Tab"},
    {TokenType::Column,         "Column"},
    {TokenType::Bool,           "Bool"},
    {TokenType::Null,           "Null"},
    {TokenType::Any,            "Any"},
    {TokenType::_EOF,           "_EOF"}
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
    char catch_escape() const;
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

enum class JsonValueType {
    Object,
    Array,
    Boolean,
    String,
    Number,
    Null
};

static const std::map<JsonValueType, const char*> JsonValueName = {
    {JsonValueType::Object, "Object"},
    {JsonValueType::Array, "Array"},
    {JsonValueType::Boolean, "Boolean"},
    {JsonValueType::String, "String"},
    {JsonValueType::Number, "Number"},
    {JsonValueType::Null, "Null"}
};

class JsonValue;
class JsonResource;

typedef std::pair<const std::string, JsonValue> JsonProperty;
typedef std::map<std::string, JsonValue> JsonObject;
typedef std::vector<JsonValue> JsonArray;

class JsonResource {

public:
    JsonResource() : _type(JsonValueType::Null) {}

    JsonResource(const std::string& value)
        : _string_value(value), _type(JsonValueType::String) {}

    JsonResource(bool value)
        : _bool_value(value), _type(JsonValueType::Boolean) {}

    JsonResource(double value)
        : _number_value(value), _type(JsonValueType::Number) {}

    JsonResource(JsonArray value)
        : _array_value(value), _type(JsonValueType::Array) {}

    JsonResource(JsonObject value)
        : _object_value(value), _type(JsonValueType::Object) {}

    JsonValueType type() const {
        return _type;
    }

    std::string name() const {
        return JsonValueName.at(_type);
    }

    JsonValueType _type;

    double _number_value;
    bool _bool_value;
    std::string _string_value;

    JsonArray _array_value;
    JsonObject _object_value;
};

class JsonValue {
    JsonResource _resource;

public:
    JsonValue() : _resource(JsonResource()) {}
    JsonValue(const JsonResource&& resource) : _resource(resource) {}

    /*
    JsonValue(const JsonValue& other) = delete;
    JsonValue& operator=(const JsonValue& other) = delete;
    */

    JsonValue& operator[](const std::string& key) {
        assert(_resource.type() == JsonValueType::Object
            && _resource._object_value.count(key) > 0 && "Can't access json object map value, bad key");

        return _resource._object_value.at(key);
    }

    JsonValue& operator[](size_t key) {
        assert(_resource.type() == JsonValueType::Array
            && _resource._array_value.size() >= key && "Can't access json array value, bad index");

        return _resource._array_value[key];
    }

    template <typename ValueType>
    void operator=(ValueType value) {
        _resource = JsonResource(std::forward<ValueType>(value));
    }

    bool& boolean() {
        assert(_resource.type() == JsonValueType::Boolean);
        return _resource._bool_value;
    }

    double& number() {
        assert(_resource.type() == JsonValueType::Number);
        return _resource._number_value;
    }

    std::string& string() {
        assert(_resource.type() == JsonValueType::String);
        return _resource._string_value;
    }

    JsonValue& at(const std::string& key) {
        assert(_resource.type() != JsonValueType::Object
            && _resource._object_value.count(key) > 0);
        return _resource._object_value.at(key);
    }

    JsonObject& object() {
        assert(_resource.type() == JsonValueType::Object);
        return _resource._object_value;
    }

    JsonValue& at(size_t index) {
        assert(_resource.type() == JsonValueType::Array
            && index <= _resource._array_value.size());
        return _resource._array_value[index];
    }

    JsonArray& array() {
        assert(_resource.type() == JsonValueType::Array);
        return _resource._array_value;
    }

    std::string serialized() const;
};

/*
This parser follows the bnf grammar shown below,
many thanks to this fella.

https://wesleytsai.io/2015/06/13/a-json-parser/
<JSON>     ::= <value>
<value>    ::= <object> | <array> | <boolean> | <string> | <number> | <null>
<array>    ::= "[" [<value>] {"," <value>}* "]"
<object>   ::= "{" [<property>] {"," <property>}* "}"
<property> ::= <string> ":" <value>
*/
class JsonParser {
    Token _current_token;
    Tokenizer _tokenizer;

public:
    JsonParser(const std::string& json_string) : _tokenizer(json_string) {
        _current_token = _tokenizer.next_token();
    }

    JsonValue parse();

private:
    JsonValue value();
    JsonProperty property();

    void parsing_error(const Token& header);
    void eat(TokenType token_type);
    void skip_useless_tokens();
};

inline
JsonValue json(double value) {
    return JsonValue(JsonResource(value));
}

inline
JsonValue json(int value) {
    return JsonValue(JsonResource((double)value));
}

inline
JsonValue json(const std::string& value) {
    return JsonValue(JsonResource(value));
}

inline
JsonValue json(const char* value) {
    std::string string_value = value;
    return JsonValue(JsonResource(string_value));
}

inline
JsonValue json(bool value) {
    return JsonValue(JsonResource(value));
}

inline
JsonValue json(JsonArray value) {
    return JsonValue(JsonResource(value));
}

inline
JsonValue json(JsonObject value) {
    return JsonValue(JsonResource(value));
}

inline
JsonValue json_null() {
    return JsonValue(JsonResource());
}

std::string JsonValue::serialized() const {
    switch (_resource.type()) {
        case JsonValueType::Number: {
            auto str = std::to_string(_resource._number_value);
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);

            if (*(str.end() - 1) == '.') {
                str = str.substr(0, str.size() - 1);
            }

            return str;
        }

        case JsonValueType::String: {
            return "\"" + _resource._string_value + "\"";
        }

        case JsonValueType::Boolean: {
            return _resource._bool_value ? "true" : "false";
        }

        case JsonValueType::Null: {
            return "null";
        }

        case JsonValueType::Object: {
            std::string serialized_object = "{";

            size_t i = 0;
            auto object_ = _resource._object_value;
            for (const auto& pair : object_) {
                serialized_object += "\"" + pair.first + "\": ";
                serialized_object += pair.second.serialized();

                if (i++ < object_.size() - 1) {
                    serialized_object += ", ";
                }
            }

            return serialized_object + "}";
        }

        case JsonValueType::Array: {
            std::string serialized_array = "[";

            size_t i = 0;
            auto array_ = _resource._array_value;
            for (const auto& value : array_) {
                serialized_array += value.serialized();

                if (i++ != array_.size() - 1) {
                    serialized_array += ", ";
                }
            }

            return serialized_array + "]";
        }
    }
}

void JsonParser::parsing_error(const Token& expected) {
    std::cerr << "\tParsing error: invalid syntax at: " << _tokenizer.pos() << "\n";
    std::cerr << "\tFound unexpected token: <" << _current_token.name() << "> ";

    switch (_current_token.type()) {
        case TokenType::Null:
            std::cerr << "with value: " << "null" << "\n";
            break;

        case TokenType::String:
            std::cerr << "with value: " << std::get<std::string>(_current_token.value()) << "\n";
            break;

        case TokenType::Number:
            std::cerr << "with value: " << std::get<double>(_current_token.value()) << "\n";
            break;

        default:
            std::cerr << "with value: " << std::get<char>(_current_token.value()) << "\n";
            break;
    }

    std::cerr << "\tExpected token: <" << expected.name() << ">\n";
    exit(1);
}

void JsonParser::skip_useless_tokens() {
    while (_current_token.type() == TokenType::WhiteSpace
        || _current_token.type() == TokenType::Tab
        || _current_token.type() == TokenType::NewLine) {
        _current_token = _tokenizer.next_token();
    }
}

void JsonParser::eat(TokenType token_type) {
    if (_current_token.type() == token_type) {
        _current_token = _tokenizer.next_token();
    } else {
        parsing_error(Token(token_type, ' '));
    }

    skip_useless_tokens();
}

JsonProperty JsonParser::property() {
    std::string pname;

    if (_current_token.type() == TokenType::String) {
        pname = std::get<std::string>(_current_token.value());
    }

    eat(TokenType::String);
    eat(TokenType::Column);

    return { pname, value() };
}

JsonValue JsonParser::value() {
    JsonResource resource;

    switch (_current_token.type()) {
        case TokenType::ObjectStart: {
            eat(TokenType::ObjectStart);
            JsonObject json_object;

            while (_current_token.type() != TokenType::ObjectEnd && _current_token.type() != TokenType::_EOF) {
                json_object.insert(property());
            }

            eat(TokenType::ObjectEnd);
            resource = JsonResource(json_object);
            break;
        }

        case TokenType::ArrayStart: {
            eat(TokenType::ArrayStart);
            JsonArray json_array;

            while (_current_token.type() != TokenType::ArrayEnd && _current_token.type() != TokenType::_EOF) {
                json_array.push_back(value());
            }

            eat(TokenType::ArrayEnd);
            resource = JsonResource(json_array);
            break;
        }

        case TokenType::Number: {
            auto token = _current_token;

            eat(TokenType::Number);
            double value = std::get<double>(token.value());
            resource = JsonResource(value);
            break;
        }

        case TokenType::String: {
            auto token = _current_token;

            eat(TokenType::String);
            std::string value = std::get<std::string>(token.value());
            resource = JsonResource(value);
            break;
        }

        case TokenType::Bool: {
            auto token = _current_token;

            eat(TokenType::Bool);
            bool value = std::get<bool>(token.value());
            resource = JsonResource(value);
            break;
        }

        case TokenType::Null: {
            auto token = _current_token;

            eat(TokenType::Null);
            resource = JsonResource();
            break;
        }
    }

    if (_current_token.type() != TokenType::ObjectEnd
        && _current_token.type() != TokenType::ArrayEnd
        && _current_token.type() != TokenType::_EOF) {
        eat(TokenType::Comma);
    }

    return JsonValue(std::move(resource));
}

JsonValue JsonParser::parse() {
    return value();
}

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
    static std::string whitespace_defs = {
        ' ',
        '\0',
        '\u0020',
        '\u000A',
        '\u000D',
        '\u0009'
    };

    while (whitespace_defs.find(_current_char) != std::string::npos) {
        advance();
    }
}

char Tokenizer::catch_escape() const {
    switch (_current_char) {
        case 'n': return '\n';
        case '\\': return '\\';
        case 'b': return '\b';
        case 'r': return '\r';
        case 'f': return '\f';
        case 't': return '\t';
        case '\"': return '\"';
        case '\'': return '\'';
        case 'u': // Should handle unicode chars...
        default: return _current_char; // just return the current char if nothing matches
    }
}

std::string Tokenizer::parse_string(char stop_at) {
    std::string parsed_string;

    while (_current_char != stop_at) {
        // Check for escapes
        if (_current_char == '\\') {
            advance();
            parsed_string += catch_escape();
            advance();
        } else {
            parsed_string += _current_char;
            advance();
        }

        if (_eof_reached) {
            parsing_error();
        }
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
}
