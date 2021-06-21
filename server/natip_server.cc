#include "jdt.hpp"
#include "jsoncpp/json/json.h"
#include "natip_server.hpp"
#include "unix_api.h"
#include <fstream>

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
                err_quit("can't found key: 'listen_port' in object 'server' in "
                         "config file %s",
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
                        printf("a new connect");
                        close(listen_fd_);

                        // 接收客户发来的配置，保存客户端的信息
                        setClientData(connfd);

                        printf("a connect disconnect\n");
                        exit(0);
                }
                close(connfd);
        }
}

// 根据来自客户端的消息设置客户数据
void NatIpServer::setClientData(int connfd) {
        Json::Value root;
        jdt::Decode decode;
        ClientData client_data;
        struct sockaddr_in addr;
        socklen_t len;
        jdt::Jdt jdt_manger;
        jdt::Msg msg;

	jdt_manger.setFd(connfd);
        msg = jdt_manger.recvMsg();
        root = msg.asJson();
        // 检查json中的信息是否正确，如果不正确则退出，并给客户发送一条错误命令
        if (!root.isMember("name")) {

                err_ret("not name data in json file");
        }

        // 如果已经有了name为root["name"]的客户端，则发送错误消息
        if (client_data_.find(root["name"].asString()) != client_data_.end()) {

                err_ret("name:%s already exist！", root["name"].asString().c_str());
        }

        // 设置client_data
        len = sizeof(struct sockaddr_in);
        if (getpeername(connfd, (SA *)&addr, &len) == -1) {
                err_ret("can't get client ip!");
        }

        client_data.name = root["name"].asString();
        client_data.addr = std::string(inet_ntoa(addr.sin_addr));
        client_data.port = ntohs(addr.sin_port);
        if (root.isMember("info")) {
                client_data.info = root["info"].asString();
        }

        // 添加name节点
        client_data_[root["name"].asString()] = client_data;

        printf("add client:\n\tname:%s\n\tip:%s\n\tport:%d\n\tinfo:%s\n",
               client_data.name.c_str(), client_data.addr.c_str(), client_data.port,
               client_data.info.c_str());
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
