#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include <fstream>
#include <iostream>

int main() {
        jdt::Encode encode;
        jdt::Decode decode;
        std::string test("it's a test\n");
        jdt::MsgBaseBody msg;

        auto data = encode.encode(test);
        if (decode.parse(data.first, data.second)) {
                msg = decode.front();
                if (msg.isValid()) {
                        if (msg.getType() == TYPE_STRING) {
				 
                        }
                }
        } else {
                std::cerr << "parse failed\n";
        }
}
