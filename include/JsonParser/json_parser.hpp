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
