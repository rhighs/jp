#pragma once

#include <map>
#include <vector>
#include <variant>
#include <string>
#include <optional>

#include <cassert>

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

typedef std::pair<std::string, JsonValue> JsonProperty;
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
    JsonValue(JsonResource resource) : _resource(resource) {}

    JsonValue(const JsonValue& other) = delete;
    JsonValue& operator=(const JsonValue& other) = delete;

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

    void operator=(bool boolean) {
    }

    void operator=(const JsonValue& boolean) {
    }

    void operator=(const JsonObject& object) {
    }

    void operator=(const JsonArray& array) {
    }

    void operator=(double number) {
    }

    void operator=(const std::string& string) {
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

    std::string serialized() const {
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