#include "jdt_connection.hpp"
#include "unix_api.h"

namespace jdt {

// 打开一个连接
void Connection::setConnection(int fd) { dup2(fd, socket_fd_); }
void Connection::openConnection(std::string hostname, std::string port) {
        socket_fd_ = Open_clientfd(hostname.c_str(), port.c_str());
}

// 关闭连接
void Connection::closeConnection() { Close(socket_fd_); }

#ifdef JSONCPP
// 发送文件
void Connection::sendData(Json::Value data) {
        Encode encode;
        uint32_t len;
        DetailType type;

        type.data_type = JSON_DATA;
        auto ptr = encode.encode(data, len, SEND_DATA, type);
        Rio_writen(socket_fd_, ptr.get(), len);
}
#endif

// 发送一条命令，命令的内容是data
void Connection::sendCmd(const std::string data, CmdType type) {
        Encode encode;
        uint32_t len;

        DetailType dt;
        dt.cmd_type = type;
        auto ptr = encode.encode(data, len, SEND_CMD, dt);
        Rio_writen(socket_fd_, ptr.get(), len);
}

// 发送一条错误消息
void Connection::sendError(const std::string data, ErrorType type) {
        Encode encode;
        uint32_t len;

        DetailType dt;
        dt.error_type = type;
        auto ptr = encode.encode(data, len, SEND_ERROR, dt);
        Rio_writen(socket_fd_, ptr.get(), len);
}

// 接收一条消息
bool Connection::recvMsg(Value msg) {
        int n;

        char buf[MAXLINE];
        while (decode_.empty()) {
                if ((n = Rio_readn(socket_fd_, buf, HeadSize)) == 0)
                        break;
                decode_.parse((uint8_t *)buf, n);
                if ((n = Rio_readn(socket_fd_, buf,
                                   decode_.getLenStillNeed())) == 0)
                        break;
        }
        if (decode_.empty())
                return false;
        msg = decode_.front();
        decode_.pop();
        return true;
}

} // namespace jdt
