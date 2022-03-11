#include <iostream>

#include "JsonParser/json_parser.hpp"

void JsonParser::parsing_error() {
    std::cerr << "Parsing error: invalid syntax at: " << _tokenizer.pos() << "\n";
    std::cerr << "Unexpected token: " << _current_token.name() << "\n";

    if (_current_token.type() == String) {
        std::cerr << "Token value: " << std::get<std::string>(_current_token.value()) << "\n";
    }

    if (_current_token.type() == _EOF) {
        std::cerr << "Token value: " << std::get<char>(_current_token.value()) << "\n";
    }

    exit(1);
}

void JsonParser::eat(TokenType token_type) {
    if (_current_token.type() == token_type) {
        std::cout << "Ate token: " << _current_token.name() << " <---- \n";
        _current_token = _tokenizer.next_token();
    } else {
        std::cout << "Expected token: " << Token(token_type, ' ').name() << "\n";
        parsing_error();
    }

    while (_current_token.type() == WhiteSpace) {
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
    JsonResource* resource;

    switch (_current_token.type()) {
        case ObjectStart: {
            eat(ObjectStart);
            JsonObject json_object;

            while (_current_token.type() != ObjectEnd && _current_token.type() != _EOF) {
                json_object.insert(property());
            }

            eat(ObjectEnd);
            resource = new JsonResource(json_object);
            break;
        }

        case ArrayStart: {
            eat(ArrayStart);
            JsonArray json_array;

            while (_current_token.type() != ArrayEnd && _current_token.type() != _EOF) {
                json_array.push_back(value());
            }

            eat(ArrayEnd);
            resource = new JsonResource(json_array);
            break;
        }

        case Integer: {
            auto token = _current_token;

            eat(Integer);
            int value = std::get<int>(token.value());
            resource = new JsonResource(value);
            break;
        }

        case String: {
            auto token = _current_token;

            eat(String);
            std::string value = std::get<std::string>(token.value());
            resource = new JsonResource(value);
            break;
        }

        case Bool: {
            auto token = _current_token;

            eat(Bool);
            bool value = std::get<bool>(token.value());
            resource = new JsonResource(value);
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

JsonValue JsonParser::parse() {
    std::cout << "Parsing json string: " << _tokenizer.text() << "\n";
    return value();
}

/*
https://wesleytsai.io/2015/06/13/a-json-parser/
<JSON>     ::= <value>
<value>    ::= <object> | <array> | <boolean> | <string> | <number> | <null>
<array>    ::= "[" [<value>] {"," <value>}* "]"
<object>   ::= "{" [<property>] {"," <property>}* "}"
<property> ::= <string> ":" <value>
*/
