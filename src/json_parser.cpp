#include <iostream>

#include "JsonParser/json_parser.hpp"

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
