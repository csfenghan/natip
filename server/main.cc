#include "data_manager.hpp"
#include "jdt_connection.hpp"
#include "natip_server.hpp"
#include <unistd.h>

// setting user information
void set_client_information(jdt::Connection &conn) {
        natip::ClientData client_data;
        jdt::Value msg;

        conn.sendCmd("please input your host name: ", jdt::STRING_MASSAGE);
        conn.recvMsg(msg);
        client_data.setName(msg.asStringData());

        conn.sendCmd("input other info:", jdt::STRING_MASSAGE);
        conn.recvMsg(msg);
        client_data.setInfo(msg.asStringData());

        client_data.setAddr(conn.getAddr());
        client_data.setPort(conn.getPort());
        client_data.setPid(getpid());
        natip::write_to_mysql(client_data);
}

int main(int argc, char **argv) {
        natip::NatIpServer server;
        jdt::Connection conn;
        int pid;

        server.loadConfig("config.json");
        server.startListen();

        // main loop
        while (true) {
                conn = server.acceptConnection();
                if ((pid = fork()) < 0) {
                        fprintf(stderr, "fork error,accept client failed!");
                        conn.sendError("accept client failed!", jdt::SERVER_SYSTEM_ERROR);
                } else if (pid == 0) {
                        server.stopListen();

                        set_client_information(conn);

                        exit(0);
                }

                conn.closeConnection();
        }
}
