#include <stdio.h>
#include <fstream>
#include <string>
#include <jp/jp.hpp>

std::string read_file_content(std::string&& filename)
{
    std::ifstream infile(filename);
    infile.seekg(0, std::ios::end);

    size_t size = infile.tellg();
    std::string stringbuffer(size, ' ');

    infile.seekg(0);
    infile.read(&stringbuffer[0], size);
    return stringbuffer;
}

int main(void)
{
    auto json_string = read_file_content("./test.json");
    auto jsonobject = jp::parse(json_string).object();
    printf("String id prop value: %s\n", jsonobject["batters"]["batter"][0]["id"].string().c_str());
    return 0;
}
