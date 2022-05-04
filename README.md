## Minimalistic json parser for C++17 and above

I've built this json parser as a lighter alternative to the ones I
found browsing on the internet... They included a lot of unnecessary stuff
as well as extra features one doesn't really look for when 
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
    // TODO
    return 0;
}

```

### Serializing
```cpp
#include <iostream>
#include <JsonParser>

int main(void)
{
    // TODO
    return 0;
}
```
