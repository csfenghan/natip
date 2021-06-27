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
        // 打开连接
        void setConnection(int fd);
        void openConnection(std::string hostname, std::string port);

        // 关闭连接
        void closeConnection();

        void sendData(std::string data);
#ifdef JSONCPP
        void sendData(Json::Value data);
#endif

        // 发送一条命令，命令的内容是data数据
        void sendCmd(const std::string data, CmdType type);

        // 发送一条错误消息
        void sendError(const std::string data, ErrorType type);

        // 接收一条消息
        bool recvMsg(Value msg);

      private:
        int socket_fd_; // 通信套接字
        Decode decode_; // 数据解析器
};

} // namespace jdt
#endif
