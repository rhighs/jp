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

class JsonData;
class JsonResource;

typedef std::pair<std::string, JsonData> JsonProperty;
typedef std::map<std::string, JsonData> JsonObject;
typedef std::vector<JsonData> JsonArray;

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
};

class JsonData {
    JsonResource* _resource;

public:
    JsonData(JsonResource* resource) : _resource(resource) {}

    ~JsonData() {
        delete _resource;
    }
};

class JsonParser {
    Token _current_token;
    Tokenizer _tokenizer;

public:
    JsonParser(Tokenizer tokenizer) : _tokenizer(tokenizer) {}

    JsonProperty property();

    void parsing_error();
    void eat(TokenType token_type);
};
