#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include <fstream>
#include <iostream>

int main() {
        jdt::Encode encode;
        jdt::Decode decode;
        std::string test1("it's a test,");
        std::string test2("maybe it's not correct,");
        std::string test3("test test...");

        uint8_t buf[1024];

        auto data1 = encode.encode(test1);
        auto data2 = encode.encode(test2);
        auto data3 = encode.encode(test3);

        decode.parse(data1.first, data1.second - 10);
        //decode.parse(data1.first + data1.second - 10, 10);
        // decode.parse(data2.first, data2.second);
        // decode.parse(data3.first, data3.second);

        while (!decode.empty()) {
                std::cout << decode.popString() << std::endl;
                //std::cout << decode.popString() << std::endl;
                //std::cout << decode.popString() << std::endl;
        }
}
