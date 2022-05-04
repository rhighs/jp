#include <iostream>
#include <variant>
#include <utility>
#include <string>
#include <fstream>

#include <curl/curl.h>

#include <JsonParser/json_parser.hpp>

namespace HTTPJson
{

static size_t write_response_buffer(void* ptr, size_t len, size_t byte_per_element, std::string* data)
{
    size_t mem_size = len * byte_per_element;
    data->append(static_cast<char*>(ptr), mem_size);
    return mem_size;
}

std::string fetch_json_data(const char* remote_json_url)
{
    std::string response_buffer;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* handle = curl_easy_init();

    if (!handle) {
        std::cout << "Bad curl handle\n";
        exit(1);
    }

    curl_easy_setopt(handle, CURLOPT_URL, remote_json_url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_response_buffer);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response_buffer);

    CURLcode response_code = curl_easy_perform(handle);
    if (response_code != CURLE_OK) {
        return "";
    }

    return response_buffer;
}

}

const char* remote_json_url =
    "https://gist.githubusercontent.com/sunilshenoy/23a3e7132c27d62599ba741bce13056a/raw/517b07fc382c843dcc7d444046d959a318695245/sample_json.json";

//https://www.json.org/json-en.html
int main(void)
{
    std::string json_remote_string = HTTPJson::fetch_json_data(remote_json_url);

    std::ifstream infile("./test/test.json");
    std::string json_string((std::istreambuf_iterator<char>(infile)),
                            (std::istreambuf_iterator<char>()));

    JsonParser parser(json_string);
    JsonValue json_object = parser.parse();

    JsonParser parser1(json_remote_string);
    JsonValue json_remote_object = parser1.parse();

    JsonValue& prop = json_remote_object["clientIdentiferData"]["clientID"];
    prop = json(100.0);
    std::cout << "clientID: " << prop.number() << "\n";

    auto o = json({
        {"name", json("test")},
        {"number", json(123)},
        {"nu\\\'mber_decimal", json(123.123)},
        {"an_array", json({
                json(true),
                json_null(),
                json("Ned Flanders"),
                json({
                    {"nested", json("some string")}
                })
            })}
    });

    o.object().insert({ "cringe", json(true) });

    auto s = o.serialized();

    std::cout << "Parsing serialized string: " << s << "\n";
    parser = JsonParser(s);
    JsonValue njson_object = parser.parse();

    JsonValue nprop = njson_object["an_array"][0];
    std::cout << "\"number\": " << nprop.boolean() << "\n";

    return 0;
}
