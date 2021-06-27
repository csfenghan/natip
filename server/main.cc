#include "jdt_connection.hpp"
#include "natip_server.hpp"
#include "unistd.h"

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
                        conn.sendError("accept client failed!",
                                       jdt::SERVER_SYSTEM_ERROR);
                } else if (pid == 0) {
                        server.stopListen();

                        exit(0);
                }

                conn.closeConnection();
        }
}
