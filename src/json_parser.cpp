#include <iostream>

#include "JsonParser/json_parser.hpp"

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
        // std::cout << "Ate token: " << _current_token.name() << " <---- \n";
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
            // Set type to null and leave all values empty
            resource = JsonResource();
            break;
        }
    }

    if (_current_token.type() != TokenType::ObjectEnd
        && _current_token.type() != TokenType::ArrayEnd
        && _current_token.type() != TokenType::_EOF) {
        eat(TokenType::Comma);
    }

    return JsonValue(resource);
}

JsonObject JsonParser::parse() {
    return value().object().value();
}
