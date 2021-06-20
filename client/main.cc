#include "natip_client.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
        NatIpClient client;
        std::string data;

        client.loadConfig("config.json");
        client.tcpInit();

	client.tcpClient();
}
