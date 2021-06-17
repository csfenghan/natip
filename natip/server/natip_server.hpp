#ifndef NATIP_SERVER_HPP
#define NATIP_SERVER_HPP

#include <string>

class NatIpServer{
	public:
		// 加载配置文件
		bool loadConfig(std::string path="config.json");

		// 等待并处理连接
		void tcpAccept();
	private:
		// 获取listen描述符
		int tcpListen();

		std::string listen_port_;
};

#endif
