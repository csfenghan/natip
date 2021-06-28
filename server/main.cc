#include "data_manager.hpp"
#include "jdt_connection.hpp"
#include "natip_server.hpp"
#include <unistd.h>
#include <vector>

// setting user information
void set_client_information(jdt::Connection &conn) {
        natip::ClientData client_data;
        jdt::Value msg;

        conn.sendString("please input your host name: ");
        conn.recvMsg(msg);
        client_data.setName(msg.asStringData());

        conn.sendString("input other info:");
        conn.recvMsg(msg);
        client_data.setInfo(msg.asStringData());

        client_data.setAddr(conn.getAddr());
        client_data.setPort(conn.getPort());
        client_data.setPid(getpid());
        natip::write_to_mysql(client_data);
}

// penetration based on user selection
void penetration(jdt::Connection &conn) {
        std::vector<natip::ClientData> data;
        natip::ClientData client_data;
        jdt::Value msg;
        char buf[1024];

        // send the host that can connect
        conn.sendString("current hostname that can connect:\n");
        natip::read_from_mysql(data);
        for (int i = 0; i < data.size(); i++) {
                sprintf(buf, "host %d\n", i);
                conn.sendString(buf);
                conn.sendString("\tname: " + data[i].getName());
                conn.sendString("\tinfo: " + data[i].getInfo());
        }
        conn.sendCmd("please input the hsot that you want to connect:", jdt::STRING_MASSAGE);
        conn.recvMsg(msg);
        if (!natip::read_from_mysql(client_data, msg.asStringData())) {
                conn.sendError("not found name:" + msg.asStringData(), jdt::FORMAT_ERROR);
                exit(0);
        }
        conn.sendString(client_data.getAddr());
        conn.sendString(std::to_string(client_data.getPort()));
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
                        penetration(conn);

                        exit(0);
                }

                conn.closeConnection();
        }
}
