#ifndef NATIP_SERVER_HPP
#define NATIP_SERVER_HPP

#include <string>
#include <vector>

class NatIpServer{
	public:
		// 加载配置文件
		bool loadConfig(std::string path="config.json");

		// 等待、处理连接
		void processConnect();
	private:
		// 客户端交互
		void echo(int connfd);

		int listen_fd_;
		std::vector<int> child_pid_;
		std::string listen_port_;
};

#endif
