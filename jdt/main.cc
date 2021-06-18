#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include <fstream>
#include <iostream>

int main() {
        jdt::Encode encode;
        Json::Value root;
        Json::Reader reader;
        std::ifstream ifs("info.json");

        if (!reader.parse(ifs, root)) {
                fprintf(stderr, "parser failed\n");
                return 0;
        }
        auto result = encode.encode(root);

        std::cout << result.second << std::endl;
}
