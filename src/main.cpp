#include <iostream>
#include <variant>
#include <utility>
#include <string>

#include "JsonParser/token.hpp"
#include "JsonParser/tokenizer.hpp"

#include "JsonParser/json_parser.hpp"

const char* json_string = "{ \"ciao\": 12, \"cringe\": true, \"object\": { \"name\": \"skumonti\" }}";

int main(void) {
    Tokenizer tokenizer(json_string);

    auto token = tokenizer.next_token();
    while (token.type() != _EOF) {
        std::cout << "Token name: " << token.name() << "\n";

        if (token.type() != String && token.type() != Bool && token.type() != Integer) {
            std::cout << "Token value: " << std::get<char>(token.value()) << "\n\n";
        }

        if (token.type() == String) {
            std::cout << "Token value: " << std::get<std::string>(token.value()) << "\n\n";
        }

        if (token.type() == Bool) {
            std::cout << "Token value: " << std::get<bool>(token.value()) << "\n\n";
        }

        if (token.type() == Integer) {
            std::cout << "Token value: " << std::get<int>(token.value()) << "\n\n";
        }

        token = tokenizer.next_token();
    }

    return 0;
}

