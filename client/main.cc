#include "jdt_connection.hpp"
#include "natip_client.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

int main(int argc, char **argv) {
        if (argc != 2) {
                fprintf(stderr, "usage: %s config file\n", argv[0]);
                return -1;
        }
        char buf[1024];

        jdt::Connection conn;
        jdt::Value msg;
        std::string input;
        natip::NatIpClient client;

        // setting client config
        client.setConfigure(argv[1]);
        conn = client.createConnection();

        conn.recvMsg(msg);
        std::cout << msg.asStringData();
        fgets(buf, 1024, stdin);
        conn.sendData(std::string(buf));

        conn.recvMsg(msg);
        std::cout << msg.asStringData();
        fgets(buf, 1024, stdin);
        conn.sendData(std::string(buf));
}
