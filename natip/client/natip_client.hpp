#ifndef NATIP_CLIENT_HPP
#define NATIP_CLIENT_HPP

#include <string>

// 内网穿透 客户端
class NatIpClient{
	public:
		// 加载配置文件
		bool loadConfig(std::string path="config.json");

		// 根据配置文件连接服务器
		void tcpInit();

		// 发送string
		void sendString(std::string data);

		// 接收string
		std::string receiveString();
	private:
		int socket_fd_;

		std::string server_addr_;
		std::string server_port_;
};

#endif
