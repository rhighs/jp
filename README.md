## Minimalistic json parser for C++17 and above

I've built this json parser as a lighter alternative to the ones I
found browsing on the internet... They included a lot of unnecessary stuff
as well as extra features one doesn't really look for when 
searching for a json parser.

### Parsing
```cpp
#include <stdio.h>
#include <jp/jp.hpp>

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
    auto jsonobject = jp::parse(awesome_json).object();
    printf("bar: %f\n", jsonobject["foo"]["bar"].number());
    return 0;
}

---
bar: 123.0
---
```

### Serializing
```cpp
#include <stdio.h>
#include <jp/jp.hpp>

const char* awesome_json =
    "{"
    "   \"foo\": {"
    "       \"bar\": 123,"
    "       \"buzz\": {"
    "           \"data\": [1, 2, 3]"
    "        }"
    "   }"
    "}";

int main(void)
{
    auto jsonobjectfoo = jp::parse(awesome_json);
    jsonobject["foo"]["bar"] = 10;
    printf("Serialized object: %s", jsonobject.serialized());
    return 0;
}

---
{
    "foo": {
        "bar": 10,
            "buzz": {
                "data": [1, 2, 3]
            }
    }
}
---
```
