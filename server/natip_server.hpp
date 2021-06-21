#ifndef NATIP_SERVER_HPP
#define NATIP_SERVER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "jsoncpp/json/json.h"

class NatIpServer {
        // 保存客户端信息
        struct ClientData {
                std::string name;
                std::string addr;
		uint16_t port;
		std::string info;
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
	void setClientData(int connfd);

	// 接收来自客户端的json文件，并保存客户信息
		

        // 信号处理函数
        static void sigchldHandler(int sig);

        int listen_fd_;           // 服务器的listen描述符
        std::string listen_port_; // 服务器监听的端口
        std::unordered_map<std::string, ClientData>
            client_data_; // 保存客户端的信息
};

#endif
