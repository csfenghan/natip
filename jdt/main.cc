#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include <fstream>
#include <iostream>

int main() {
        jdt::Encode encode;
        jdt::Decode decode;
        std::string test("it's a test");

        auto data = encode.encode(test);
        if (!decode.parse(data.first, data.second)) {
                std::cerr << "parse failed";
                return 0;
        }

        if (!decode.empty()) {
                if (decode.nextIsString())
                        std::cout << decode.getString() << std::endl;
        }
}
