#include "data_manager.hpp"
#include "natip_server.hpp"
#include "unix_api.h"

#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#ifdef JSONCPP
#include "jsoncpp/json/json.h"
#endif
#include "mysql_connection.h"

#include <fstream>

namespace natip {

NatIpServer::NatIpServer() { Signal(SIGCHLD, sigchldHandler); }

NatIpServer::~NatIpServer() {}

// load the config file
void NatIpServer::loadConfig(std::string path) {
        Json::Value root, server;
        Json::Reader reader;
        std::ifstream in_file_stream;

        // read config file
        in_file_stream.open(path, std::ifstream::in);
        if (!in_file_stream.is_open())
                err_quit("can't open file: %s", path.c_str());

        if (!reader.parse(in_file_stream, root))
                err_quit("can't parse file: %s", path.c_str());

        // configure
        if (!root.isMember("server"))
                err_quit("can't found key: 'server' in config file %s", path.c_str());
        server = root["server"];

        if (!server.isMember("listen_port"))
                err_quit("can't found key: 'listen_port' in object 'server' in "
                         "config file %s",
                         path.c_str());

        listen_port_ = server["listen_port"].asString();
        in_file_stream.close();
}

// start listen
void NatIpServer::startListen() {
        listen_fd_ = Open_listenfd(listen_port_.c_str());
}

// stop listen
void NatIpServer::stopListen() { close(listen_fd_); }

// accpet and return a Connection object
jdt::Connection NatIpServer::acceptConnection() {
        jdt::Connection conn;
        int connfd;

        while ((connfd = accept(listen_fd_, NULL, NULL)) < 0) {
                if (errno == EINTR)
                        continue;
                else
                        err_sys("accept error");
        }
        conn.setConnection(connfd);
	close(connfd);

        return conn;
}
// chld信号处理函数
void NatIpServer::sigchldHandler(int sig) {
        int old_errno;
        int status;
        pid_t pid;

        old_errno = errno;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
                // 删除对应pid的信息
                if (!del_from_mysql(pid)) {
                        sio_puts((char *)"Failed to delete client information "
                                         "for pid: ");
                        sio_putl(pid);
                } else {
                        sio_puts((char *)"Successfully remove a client\n");
                }
        }

        errno = old_errno;
}

// int信号处理函数
void NatIpServer::sigintHandler(int sig) {}
} // namespace natip
