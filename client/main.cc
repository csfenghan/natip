#include "natip_client.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
        if (argc != 2) {
                fprintf(stderr, "usage: %s config file\n", argv[0]);
                return -1;
        }
        NatIpClient client;
        std::string data;

        client.loadConfig(std::string(argv[1]));
        client.tcpInit();

        client.tcpClient();
}
