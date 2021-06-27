/************************************************************
 *	本文件定义了Jdt收发通信的通信接口
 ***********************************************************/

#ifndef JDT_CONNECTION_HPP
#define JDT_CONNECTION_HPP

#include "jdt_body.hpp"
#include "jdt_parse.hpp"
#include "jdt_value.hpp"
#include <string>

namespace jdt {
class Connection {
      public:
        // open connect
        void setConnection(int fd);
        void openConnection(std::string hostname, std::string port);

        // close connect
        void closeConnection();

        void sendData(std::string data) const;
#ifdef JSONCPP
        void sendData(Json::Value data) const;
#endif

        // send a command
        void sendCmd(const std::string data, CmdType type);

        // send a error
        void sendError(const std::string data, ErrorType type);

        // receive a msg
        bool recvMsg(Value &msg);

        // get addr
        std::string getAddr() { return addr_; }
        int getPort() { return port_; }

      private:
        int socket_fd_;    // 通信套接字
        std::string addr_; // 对面的地址
        int port_; // 对面的端口
        Decode decode_;    // 数据解析器
};

} // namespace jdt
#endif
