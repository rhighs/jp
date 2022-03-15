#pragma once

#include <map>
#include <vector>
#include <variant>
#include <string>
#include <optional>

#include "JsonParser/token.hpp"
#include "JsonParser/tokenizer.hpp"

enum class JsonValueType {
    Object,
    Array,
    Boolean,
    String,
    Number,
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
        : _number_value(value), _type(JsonValueType::Number) {}

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
            case JsonValueType::Number:
                return "Number";
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

    std::optional<bool> boolean() const {
        if (_resource.type() != JsonValueType::Boolean) {
            return {};
        }

        return _resource._bool_value;
    }

    std::optional<double> number() const {
        if (_resource.type() != JsonValueType::Number) {
            return {};
        }

        return _resource._number_value;
    }

    std::optional<std::string> string() const {
        if (_resource.type() != JsonValueType::String) {
            return {};
        }

        return _resource._string_value;
    }

    std::optional<JsonValue> at(const std::string& key) const {
        if (_resource.type() != JsonValueType::Object
            || _resource._object_value.find(key) == _resource._object_value.end()) {
            return {};
        }

        return _resource._object_value.at(key);
    }

    std::optional<JsonObject> object() const {
        if (_resource.type() != JsonValueType::Object) {
            return {};
        }

        return _resource._object_value;
    }

    std::optional<JsonValue> at(size_t index) const {
        if (_resource.type() != JsonValueType::Array
            || index >= _resource._array_value.size()) {
            return {};
        }

        return _resource._array_value[index];
    }

    std::optional<JsonArray> array() const {
        if (_resource.type() != JsonValueType::Array) {
            return {};
        }

        return _resource._array_value;
    }

    std::string serialized() const {
        switch (_resource.type()) {
            case JsonValueType::Number: {
                return std::to_string(number().value());
            }

            case JsonValueType::String: {
                return "\"" + string().value() + "\"";
            }

            case JsonValueType::Boolean: {
                return boolean().value() ? "true" : "false";
            }

            case JsonValueType::Null: {
                return "null";
            }

            case JsonValueType::Object: {
                std::string serialized_object = "{";

                size_t i = 0;
                auto object_ = object().value();
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
                auto array_ = array().value();
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

    JsonObject parse();

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
JsonValue json(const std::string& value) {
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