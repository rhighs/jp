#include <iostream>

#include "JsonParser/json_parser.hpp"

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

void JsonParser::eat(TokenType token_type) {
    if (_current_token.type() == token_type) {
        _current_token = _tokenizer.next_token();
    } else {
        parsing_error(Token(token_type, ' '));
    }

    skip_useless_tokens();
}

void JsonParser::skip_useless_tokens() {
    while (_current_token.type() == TokenType::WhiteSpace
        || _current_token.type() == TokenType::Tab
        || _current_token.type() == TokenType::NewLine) {
        _current_token = _tokenizer.next_token();
    }
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
