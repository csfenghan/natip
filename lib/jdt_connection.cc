#include "jdt_connection.hpp"
#include "unix_api.h"

namespace jdt {

// open a connect
void Connection::setConnection(int fd) {
        socklen_t len;
        sockaddr_in addr;

        socket_fd_ = dup(fd);

        if (getpeername(socket_fd_, (SA *)&addr, &len) == -1)
                err_ret("getperrname() error");
        addr_ = inet_ntoa(addr.sin_addr);
        port_ = ntohs(addr.sin_port);
}
void Connection::openConnection(std::string hostname, std::string port) {
        socket_fd_ = Open_clientfd(hostname.c_str(), port.c_str());
}

// close connect
void Connection::closeConnection() { Close(socket_fd_); }

// send file
void Connection::sendString(std::string data) const {
        Encode encode;
        uint32_t len;
        DetailType type;

        type.data_type = STRING_DATA;
        auto ptr = encode.encode(data, len, SEND_DATA, type);
        Rio_writen(socket_fd_, ptr.get(), len);
}
#ifdef JSONCPP
// send file
void Connection::sendData(Json::Value data) const {
        Encode encode;
        uint32_t len;
        DetailType type;

        type.data_type = JSON_DATA;
        auto ptr = encode.encode(data, len, SEND_DATA, type);
        Rio_writen(socket_fd_, ptr.get(), len);
}
#endif

// send a command
void Connection::sendCmd(const std::string data, CmdType type) {
        Encode encode;
        uint32_t len;

        DetailType dt;
        dt.cmd_type = type;
        auto ptr = encode.encode(data, len, SEND_CMD, dt);
        Rio_writen(socket_fd_, ptr.get(), len);
}

// send error
void Connection::sendError(const std::string data, ErrorType type) {
        Encode encode;
        uint32_t len;

        DetailType dt;
        dt.error_type = type;
        auto ptr = encode.encode(data, len, SEND_ERROR, dt);
        Rio_writen(socket_fd_, ptr.get(), len);
}

// receive a msg
bool Connection::recvMsg(Value &msg) {
        int n;

        char buf[MAXLINE];
        while (decode_.empty()) {
                if ((n = Rio_readn(socket_fd_, buf, HeadSize)) == 0)
                        break;
                decode_.parse((uint8_t *)buf, n);
                if ((n = Rio_readn(socket_fd_, buf, decode_.getLenStillNeed())) == 0)
                        break;
                decode_.parse((uint8_t *)buf, n);
        }
        if (decode_.empty()) {
                fprintf(stderr, "decode error,decocde_ is empty\n");
                return false;
        }
        msg = decode_.front();
        decode_.pop();
        return true;
}

} // namespace jdt
