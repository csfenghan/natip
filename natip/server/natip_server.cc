#include "natip_server.hpp"
#include "unix_api.h"
#include <fstream>
#include <jsoncpp/json/json.h>

// 初始化函数
void NatIpServer::tcpInit() {
        // 1.设置信号处理函数
        Signal(SIGCHLD, sigchldHandler);
}

// 加载json配置文件
void NatIpServer::loadConfig(std::string path) {
        Json::Value root, server;
        Json::Reader reader;
        std::ifstream in_file_stream;

        // 1.读取config文件
        in_file_stream.open(path, std::ifstream::in);
        if (!in_file_stream.is_open())
                err_quit("can't open file: %s", path.c_str());

        if (!reader.parse(in_file_stream, root))
                err_quit("can't parse file: %s", path.c_str());

        // 2.使用config文件进行配置
        if (!root.isMember("server"))
                err_quit("can't found key: 'server' in config file %s", path.c_str());
        server = root["server"];

        if (!server.isMember("listen_port"))
                err_quit("can't found key: 'listen_port' in object 'server' in config file %s",
                         path.c_str());

        listen_port_ = server["listen_port"].asString();
}

// 服务器主循环
void NatIpServer::tcpServer() {
        int connfd;
        pid_t pid;
        socklen_t client_len;
        struct sockaddr_storage client_addr;

        // 1.bind并listen
        listen_fd_ = Open_listenfd(listen_port_.c_str());

        while (true) {
                // 2.accept连接，并处理被信号中断的情况
                client_len = sizeof(client_addr);
                if ((connfd = accept(listen_fd_, (SA *)&client_addr, &client_len)) < 0) {
                        if (errno == EINTR)
                                continue;
                        else
                                err_sys("accept error");
                }

                // 3.子进程处理服务
                if ((pid = Fork()) == 0) {
                        printf("a new tcp connect\n");
                        close(listen_fd_);

                        echo(connfd);

                        printf("a tcp connect disconnect\n");
                        exit(0);
                }
                close(connfd);
        }
}

// 服务函数
void NatIpServer::echo(int connfd) {
        size_t n;
        char buf[MAXLINE];
        rio_t rio;

        Rio_readinitb(&rio, connfd);
        while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
                printf("server received %d bytes\n", (int)n);
                Rio_writen(connfd, buf, n);
        }
}

// chld信号处理函数
void NatIpServer::sigchldHandler(int sig) {
        int old_errno;
        int status;
        pid_t pid;

        old_errno = errno;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
                sio_puts((char *)"pid: ");
                sio_putl(pid);
                sio_puts((char *)" died\n");
        }

        errno = old_errno;
}
