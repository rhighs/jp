#pragma once

#include <map>
#include <vector>
#include <variant>
#include <optional>

#include "JsonParser/token.hpp"
#include "JsonParser/tokenizer.hpp"

enum class JsonValueType {
    Object,
    Array,
    Boolean,
    String,
    Integer,
    Null
};

class JsonValue;
class JsonResource;

typedef std::pair<std::string, JsonValue> JsonProperty;
typedef std::map<std::string, JsonValue> JsonObject;
typedef std::vector<JsonValue> JsonArray;

class JsonResource {

public:
    JsonResource() : _type(JsonValueType::Null) {}

    JsonResource(std::string value)
        : _string_value(value), _type(JsonValueType::String) {}

    JsonResource(double value)
        : _number_value(value), _type(JsonValueType::Integer) {}

    JsonResource(JsonArray value)
        : _array_value(value), _type(JsonValueType::Array) {}

    JsonResource(JsonObject value)
        : _object_value(value), _type(JsonValueType::Object) {}

    JsonValueType type() const {
        return _type;
    }

    std::string name() const {
        switch (_type) {
            case JsonValueType::Object:
                return "Object";
            case JsonValueType::Array:
                return "Array";
            case JsonValueType::Integer:
                return "Integer";
            case JsonValueType::Boolean:
                return "Boolean";
            case JsonValueType::String:
                return "String";
            case JsonValueType::Null:
                return "Null";
        }
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
    JsonValue(JsonResource resource) : _resource(resource) {}

    std::optional<bool> boolean() {
        if (_resource.type() != JsonValueType::Boolean) {
            return {};
        }

        return _resource._bool_value;
    }

    std::optional<double> number() {
        if (_resource.type() != JsonValueType::Integer) {
            return {};
        }

        return _resource._number_value;
    }

    std::optional<std::string> string() {
        if (_resource.type() != JsonValueType::String) {
            return {};
        }

        return _resource._string_value;
    }

    std::optional<JsonValue> at(const std::string& key) {
        if (_resource.type() != JsonValueType::Object
            || _resource._object_value.find(key) == _resource._object_value.end()) {
            return {};
        }

        return _resource._object_value.at(key);
    }

    std::optional<JsonValue> at(size_t index) {
        if (_resource.type() != JsonValueType::Array
            || index >= _resource._array_value.size()) {
            return {};
        }

        return _resource._array_value[index];
    }
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
