#include <iostream>
#include <variant>
#include <utility>
#include <string>
#include <fstream>

#include <JsonParser/json_parser.hpp>

int main(void) {
    std::ifstream infile("./test/test.json");
    std::string json_string((std::istreambuf_iterator<char>(infile)),
                            (std::istreambuf_iterator<char>()));

    JsonParser parser(json_string);
    JsonObject json_object = parser.parse();

    auto bobject = json_object.at("batters");
    if (auto barray = bobject.at("batter")) {
        if (auto bobject = barray.value().at(0)) {
            if (auto id = bobject.value().at("id")) {
                std::cout << "\"id\": " << id.value().string().value() << "\n";
            }
        }
    }

    auto s = json({
        {"name", json("test")},
        {"number", json(123)},
        {"number_decimal", json(123.123)},
        {"an_array", json({
                json(true),
                json_null(),
                json("Ned Flanders"),
                json({
                    {"nested", json("some string")}
                })
            })}
    }).serialized();

    std::cout << "Parsing serialized string: " << s << "\n";
    parser = JsonParser(s);
    json_object = parser.parse();

    auto prop = json_object.at("number");
    std::cout << "\"number\": " << prop.number().value() << "\n";

    return 0;
}
