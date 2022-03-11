#include <iostream>

#include "JsonParser/json_parser.hpp"

void JsonParser::parsing_error() {
    std::cerr << "Parsing error: invalid syntax\n";
    exit(1);
}

void JsonParser::eat(TokenType token_type) {
    if (_current_token.type() == token_type) {
        _current_token = _tokenizer.next_token();
    } else {
        parsing_error();
    }

    while (_current_token.type() == WhiteSpace) {
        _current_token = _tokenizer.next_token();
    }
}

JsonProperty JsonParser::property() {
    eat(String);
    JsonProperty p = {"name", JsonData(new JsonResource(12))};
    eat(Integer);
}

/*
https://wesleytsai.io/2015/06/13/a-json-parser/
<JSON>     ::= <value>
<value>    ::= <object> | <array> | <boolean> | <string> | <number> | <null>
<array>    ::= "[" [<value>] {"," <value>}* "]"
<object>   ::= "{" [<property>] {"," <property>}* "}"
<property> ::= <string> ":" <value>
*/
