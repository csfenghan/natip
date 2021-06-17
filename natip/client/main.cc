#include "natip_client.hpp"
#include <iostream>

int main(int argc, char **argv) {
        NatIpClient client;

        client.loadConfig("config.json");
        if (client.tcpConnect() < 0) {
		perror("connect failed");
                return -1;
        }
}
