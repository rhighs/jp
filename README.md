## A simple and lightweight json parser written in C++17

I've built this json parser as a lighter alternative to the ones I
found browsing on the internet... They included a lot of unnecessary stuff
as well as extra functionalities one's doesn't really looks for when 
searching for a json parser.

### Parsing
```cpp
#include <iostream>
#include <JsonParser>

const char* awesome_json =
    "{"
    "   \"foo\": {"
    "       \"bar\": 123,"
    "       \"buzz\": {"
    "           \"data\": [1, 2, 3]"
    "        }"
    "   },"
    "   \"fizz\": {"
    "       \"buzz\": 123,"
    "       \"fizzbuzz\": {"
    "           \"data\": [4, 5, 6]"
    "        }"
    "   }"
    "}";

int main(void)
{
    JsonParser jp(awesome_json);
    JsonValue jv = jp.parse();

    if (auto foo_object = jv.at("foo")) {
        if (auto bar_value = *(foo_object).at("bar")) {
            // Prints 123
            std::cout << *(bar_value.number()) << "\n";
        }
    }

    return 0;
}

```

### Serializing
```cpp
#include <iostream>
#include <JsonParser>

// WARNING: Needs improvements, don't use it :)
int main(void)
{
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

    std::cout << s << "\n";

    return 0;
}
```
