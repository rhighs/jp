#pragma once

#include <map>
#include <vector>
#include <variant>

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
    JsonValueType _type;

    int _int_value;
    bool _bool_value;
    std::string _string_value;

    JsonArray _array_value;
    JsonObject _object_value;

public:
    JsonResource() : _type(JsonValueType::Null) {}

    JsonResource(std::string value)
        : _string_value(value), _type(JsonValueType::String) {}

    JsonResource(int value)
        : _int_value(value), _type(JsonValueType::Integer) {}

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
};

class JsonValue {
    JsonResource* _resource;

public:
    JsonValue() : _resource(new JsonResource()) {}
    JsonValue(JsonResource* resource) : _resource(resource) {}

    ~JsonValue() {
        //delete _resource;
    }

    const JsonResource* resource() const {
        return _resource;
    }
};

class JsonParser {
    Token _current_token;
    Tokenizer _tokenizer;

public:
    JsonParser(Tokenizer tokenizer) : _tokenizer(tokenizer) {
        _current_token = _tokenizer.next_token();
    }

    JsonValue parse();
    JsonValue value();
    JsonProperty property();

    void parsing_error();
    void eat(TokenType token_type);
};
