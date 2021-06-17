#include "natip_client.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
        NatIpClient client;
        std::string data;

        client.loadConfig("config.json");
        client.tcpInit();

        while (1) {
                std::cout << "pleas input something:";
                fflush(stdout);
                std::cin >> data;
		client.sendString(data);
        }
}
