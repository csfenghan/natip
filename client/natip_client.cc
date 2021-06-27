#include "jdt_connection.hpp"
#ifdef JSONCPP
#include "jsoncpp/json/json.h"
#endif
#include "natip_client.hpp"
#include "unix_api.h"
#include <fstream>
#include <iostream>

namespace natip {

// 构造函数
NatIpClient::NatIpClient() {}

// 从配置文件中加载配置
void NatIpClient::setConfigure(std::string path) {
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
                err_quit("can't found key: 'server' in config file %s",
                         path.c_str());
        server = root["server"];

        if (!server.isMember("addr"))
                err_quit("can't found key: 'addr' in object 'server' in config "
                         "file %s",
                         path.c_str());
        if (!server.isMember("port"))
                err_quit("can't found key: 'port' in object 'server' in config "
                         "file %s",
                         path.c_str());

        server_addr_ = server["addr"].asString();
        server_port_ = server["port"].asString();
}

// 创建一个连接节点，返回一个jdt对象，可以直接和服务器通信
jdt::Connection NatIpClient::createConnection() {
        jdt::Connection node;
        int socket_fd;

        socket_fd = Open_clientfd(server_addr_.c_str(), server_port_.c_str());
        node.setConnection(socket_fd);
        return node;
}

} // namespace natip
