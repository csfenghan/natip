#ifndef NATIP_SERVER_HPP
#define NATIP_SERVER_HPP

#include <string>
#include <unordered_map>
#include <vector>

class NatIpServer {
        // 保存客户端信息
        struct ClientData {
                std::string name;
                std::string addr;
                std::string port;
        };
        /***************************************
         *	public
         **************************************/
      public:
        // 初始化
        void tcpInit();

        // 加载配置文件
        void loadConfig(std::string path = "config.json");

        // 提供服务
        void tcpServer();

        /***************************************
         *	private
         **************************************/
      private:
        // 客户端交互
        void echo(int connfd);

        // 信号处理函数
        static void sigchldHandler(int sig);

        int listen_fd_;           // 服务器的listen描述符
        std::string listen_port_; // 服务器监听的端口
        std::unordered_map<std::string, ClientData>
            client_data_; // 保存客户端的信息
};

#endif
