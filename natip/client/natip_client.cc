#include "natip_client.hpp"
#include <fstream>
#include <iostream>

#include <jsoncpp/json/json.h>
#include <unix_api.h>

bool NatIpClient::loadConfig(std::string path) {
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

        if (!server.isMember("addr"))
                err_quit("can't found key: 'addr' in object 'server' in config file %s",
                         path.c_str());
        if (!server.isMember("port"))
                err_quit("can't found key: 'port' in object 'server' in config file %s",
                         path.c_str());

        server_addr_ = server["addr"].asString();
        server_port_ = server["port"].asString();

        return true;
}

int NatIpClient::tcpConnect() {
        int clientfd;
        struct addrinfo hints, *listp, *p;

        // 1.域名、服务名解析
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_socktype = SOCK_STREAM; //返回的套接字可以作为连接的端点使用
        hints.ai_flags = AI_ADDRCONFIG;  //只有当主机被配置为IPv4时，才返回IPv4地址
        Getaddrinfo(server_addr_.c_str(), server_port_.c_str(), &hints, &listp);

        // 2.连接服务器
        for (p = listp; p; p = p->ai_next) {
                if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
                        continue;
                if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
                        break;
                Close(clientfd);
        }

        // 3.结束
        freeaddrinfo(listp);
        if (!p) //连接失败
                return -1;
        else
                return clientfd;
}
