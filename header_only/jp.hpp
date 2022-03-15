#include <iostream>

#include <map>
#include <vector>
#include <variant>
#include <string>
#include <optional>

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
    Null,
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
            case Null:
                return "Null";
            default:
                return "Any";
        }
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

    /*
        explicit JsonValue(JsonObject object) : _resource(object) {}
        explicit JsonValue(JsonArray array) : _resource(array) {}
        explicit JsonValue(std::string string) : _resource(string) {}
        explicit JsonValue(double number) : _resource(number) {}
        explicit JsonValue(bool boolean) : _resource(boolean) {}
    */

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
                auto str = std::to_string(number().value());
                str.erase(str.find_last_not_of('0') + 1, std::string::npos);

                if (*(str.end() - 1) == '.') {
                    str = str.substr(0, str.size() - 1);
                }

                return str;
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
JsonObject JsonParser::parse() {
    return value().object().value();
}

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

void JsonParser::parsing_error(const Token& expected) {
    std::cerr << "\tParsing error: invalid syntax at: " << _tokenizer.pos() << "\n";
    std::cerr << "\tFound unexpected token: <" << _current_token.name() << "> ";

    switch (_current_token.type()) {
        case Null:
            std::cerr << "with value: " << "null" << "\n";
            break;

        case String:
            std::cerr << "with value: " << std::get<std::string>(_current_token.value()) << "\n";
            break;

        case Number:
            std::cerr << "with value: " << std::get<double>(_current_token.value()) << "\n";
            break;

        default:
            std::cerr << "with value: " << std::get<char>(_current_token.value()) << "\n";
            break;
    }
    std::cerr << "\tExpected token: <" << expected.name() << ">\n";

    exit(1);
}

void JsonParser::eat(TokenType token_type) {
    if (_current_token.type() == token_type) {
        // std::cout << "Ate token: " << _current_token.name() << " <---- \n";
        _current_token = _tokenizer.next_token();
    } else {
        parsing_error(Token(token_type, ' '));
    }

    skip_useless_tokens();
}

void JsonParser::skip_useless_tokens() {
    while (_current_token.type() == WhiteSpace
        || _current_token.type() == Tab
        || _current_token.type() == NewLine) {
        _current_token = _tokenizer.next_token();
    }
}

JsonProperty JsonParser::property() {
    std::string pname;

    if (_current_token.type() == String) {
        pname = std::get<std::string>(_current_token.value());
    }

    eat(String);
    eat(Column);

    return { pname, value() };
}

JsonValue JsonParser::value() {
    JsonResource resource;

    switch (_current_token.type()) {
        case ObjectStart: {
            eat(ObjectStart);
            JsonObject json_object;

            while (_current_token.type() != ObjectEnd && _current_token.type() != _EOF) {
                json_object.insert(property());
            }

            eat(ObjectEnd);
            resource = JsonResource(json_object);
            break;
        }

        case ArrayStart: {
            eat(ArrayStart);
            JsonArray json_array;

            while (_current_token.type() != ArrayEnd && _current_token.type() != _EOF) {
                json_array.push_back(value());
            }

            eat(ArrayEnd);
            resource = JsonResource(json_array);
            break;
        }

        case Number: {
            auto token = _current_token;

            eat(Number);
            double value = std::get<double>(token.value());
            resource = JsonResource(value);
            break;
        }

        case String: {
            auto token = _current_token;

            eat(String);
            std::string value = std::get<std::string>(token.value());
            resource = JsonResource(value);
            break;
        }

        case Bool: {
            auto token = _current_token;

            eat(Bool);
            bool value = std::get<bool>(token.value());
            resource = JsonResource(value);
            break;
        }

        case Null: {
            auto token = _current_token;

            eat(Null);
            // Set type to null and leave all values empty
            resource = JsonResource();
            break;
        }
    }

    // If none of these ifs is satisfied, EOF was reached
    if (_current_token.type() == _EOF) {
        return JsonValue(resource);
    }

    if (_current_token.type() != ObjectEnd && _current_token.type() != ArrayEnd) {
        eat(Comma);
    }

    return JsonValue(resource);
}

JsonObject JsonParser::parse() {
    return value().object().value();
}
#include <string>
#include <iostream>

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
    while (_current_char == ' ') {
        advance();
    }
}

std::string Tokenizer::parse_string(char stop_at) {
    std::string parsed_string;

    while (_current_char != stop_at) {
        if (_eof_reached) {
            parsing_error();
        }

        parsed_string += _current_char;
        advance();
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
            return Token(Number, parse_number());
        }

        Token t;
        switch (_current_char) {
            case '\'': 
            case '\"': {
                char stop_at = _current_char;
                advance();
                t = Token(String, parse_string(stop_at));
                break;
            }
            case ',': {
                t = Token(Comma, _current_char);
                break;
            }
            case '[': {
                t = Token(ArrayStart, _current_char);
                break;
            }
            case ']': {
                t =  Token(ArrayEnd, _current_char);
                break;
            }
            case '{': {
                t = Token(ObjectStart, _current_char);
                break;
            }
            case '}': {
                t = Token(ObjectEnd, _current_char);
                break;
            }
            case ':': {
                t = Token(Column, _current_char);
                break;
            }
            case ' ': {
                t = Token(WhiteSpace, _current_char);
                break;
            }
            case '\n': {
                t = Token(NewLine, _current_char);
                break;
            }
            case '\t': {
                t = Token(Tab, _current_char);
                break;
            }
            default: {
                if (can_be_bool()) {
                    t = Token(Bool, parse_bool());
                } else if (can_be_null()) {
                    t = Token(Null, parse_null());
                }

                break;
            }
        }

        advance();
        return t;
    }

    return Token(_EOF, ' ');
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
