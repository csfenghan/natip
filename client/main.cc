#include "jdt_connection.hpp"
#include "natip_client.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

void send_client_data(jdt::Connection &conn) {
        jdt::Value msg;
        char buf[1024];

        conn.recvMsg(msg);
        std::cout << msg.asStringData();
        fgets(buf, 1024, stdin);
        conn.sendString(std::string(buf));

        conn.recvMsg(msg);
        std::cout << msg.asStringData();
        fgets(buf, 1024, stdin);
        conn.sendString(std::string(buf));
}

void select_obj_host(jdt::Connection &conn) {
        jdt::Value msg;
        char buf[1024];

        do {
                conn.recvMsg(msg);
                std::cout << msg.asStringData();
        } while (!msg.isCmd());
}

int main(int argc, char **argv) {
        if (argc != 2) {
                fprintf(stderr, "usage: %s config file\n", argv[0]);
                return -1;
        }
        natip::NatIpClient client;
        jdt::Connection conn;

        // setting client config
        client.setConfigure(argv[1]);
        conn = client.createConnection();

        send_client_data(conn);
        select_obj_host(conn);
}
