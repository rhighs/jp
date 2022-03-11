#include <iostream>
#include <variant>
#include <utility>
#include <string>

#include "JsonParser/token.hpp"
#include "JsonParser/tokenizer.hpp"

#include "JsonParser/json_parser.hpp"

const char* json_string =
    //"{\"ciao\": 12.012, \"cringe\": true, \"object\": { \"name\": \"test\" }, \"some_data\": [1, 2, 3]}";
"{\n"
"    \"quiz\": {\n"
"        \"sport\": {\n"
"            \"q1\": {\n"
"                \"question\": \"Which one is correct team name in NBA?\",\n"
"                \"options\": [\n"
"                    \"New York Bulls\",\n"
"                    \"Los Angeles Kings\",\n"
"                    \"Golden State Warriros\",\n"
"                    \"Huston Rocket\"\n"
"                ],\n"
"                \"answer\": \"Huston Rocket\"\n"
"            }\n"
"        },\n"
"        \"maths\": {\n"
"            \"q1\": {\n"
"                \"question\": \"5 + 7 = ?\",\n"
"                \"options\": [\n"
"                    \"10\",\n"
"                    \"11\",\n"
"                    \"12\",\n"
"                    \"13\"\n"
"                ],\n"
"                \"answer\": \"12\"\n"
"            },\n"
"            \"q2\": {\n"
"                \"question\": \"12 - 8 = ?\",\n"
"                \"options\": [\n"
"                    \"1\",\n"
"                    \"2\",\n"
"                    \"3\",\n"
"                    \"4\"\n"
"                ],\n"
"                \"answer\": \"4\"\n"
"            }\n"
"        }\n"
"    }\n"
"}";

int main(void) {
    Tokenizer tokenizer(json_string);
    JsonParser parser(tokenizer);
    auto value = parser.parse();

    if (auto quiz = value.at("quiz")) {
        if (auto sport = quiz.value().at("sport")) {
            if (auto q1 = sport.value().at("q1")) {
                if (auto options = q1.value().at("options")) {
                    if (auto value = options.value().at(1)) {
                        if (auto number = value.value().string()) {
                            std::cout << number.value() << std::endl;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
