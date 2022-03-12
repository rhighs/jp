#include <iostream>
#include <variant>
#include <utility>
#include <string>
#include <fstream>

#include "JsonParser/json_parser.hpp"

int main(void) {
    std::ifstream infile("./test/test.json");
    std::string json_string((std::istreambuf_iterator<char>(infile)),
                            (std::istreambuf_iterator<char>()));

    JsonParser parser(json_string);
    JsonValue value = parser.parse();

    if (auto batters = value.at("batters")) {
        if (auto barray = batters.value().at("batter")) {
            if (auto bobject = barray.value().at(0)) {
                if (auto id = bobject.value().at("id")) {
                    std::cout << id.value().string().value() << std::endl;
                }
            }
        }
    }

    return 0;
}
