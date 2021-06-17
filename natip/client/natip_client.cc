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

void NatIpClient::tcpInit() {
        socket_fd_ = Open_clientfd(server_addr_.c_str(), server_port_.c_str());
}

void NatIpClient::sendString(std::string data) {
        Rio_writen(socket_fd_, data.c_str(), data.size());
}

std::string NatIpClient::receiveString() {
        char buf[MAXLINE];
        std::string result;

        Rio_readn(socket_fd_, buf, MAXLINE);
        result = std::string(buf);

	return result;
}
