#ifndef NATIP_CLIENT_HPP
#define NATIP_CLIENT_HPP

#include "jdt_connection.hpp"
#include <string>

namespace natip {
// 内网穿透 客户端
class NatIpClient {
      public:
        // 构造函数
        NatIpClient();

        // 加载配置文件
        void setConfigure(std::string path);

        // 创建一个连接节点，返回一个jdt对象，可以直接和服务器通信
        jdt::Connection createConnection();

      private:
        std::string server_addr_;
        std::string server_port_;
};

} // namespace natip
#endif
