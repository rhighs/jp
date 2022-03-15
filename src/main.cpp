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
    JsonObject object = parser.parse();

    auto bobject = object.at("batters");
    if (auto barray = bobject.at("batter")) {
        if (auto bobject = barray.value().at(0)) {
            if (auto id = bobject.value().at("id")) {
                std::cout << id.value().string().value() << "\n";
            }
        }
    }

    auto s = json({
        {"name", json("test")},
        {"number", json(123)},
        {"number_decimal", json(123.123)},
        {"an_array", json({
                json(true),
                json_null(/*A null value*/),
                json("Ned_Flanders"),
                json({
                    {"nested", json("some string")}
                })
            })}
    }).serialized();

    std::cout << "Deserializing serialized string: " << s << "\n";

    JsonParser parser_(s);
    auto json_object = parser_.parse();

    auto prop1_value = json_object.at("number");
    std::cout << "hello prop value: " << prop1_value.number().value() << "\n";

    return 0;
}
