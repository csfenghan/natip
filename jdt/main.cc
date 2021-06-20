#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include <fstream>
#include <iostream>

int main() {
        Json::Reader reader;
        Json::Value root;
        Json::StyledWriter swriter;
        jdt::Encode encode;
        jdt::Decode decode;
        std::ifstream ifs;
        std::ofstream ofs;

        ifs.open("info.json", std::fstream::in);
        if (!reader.parse(ifs, root)) {
                std::cerr << "parse json failed" << std::endl;
                return -1;
        }

        auto data_encoded = encode.encode(root);

        decode.parse(data_encoded.first, data_encoded.second);
        if (decode.empty()) {
                std::cerr << "decode parse failed" << std::endl;
                return -1;
        }
        Json::Value data_decoded = decode.popJson();
        ofs.open("output.json");
        ofs << swriter.write(data_decoded);
        ofs.close();
        if (decode.empty()) {
                std::cout << "parse normal" << std :: endl;
        }
}
