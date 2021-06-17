#include <string>

// 内网穿透 客户端
class NatIpClient{
	public:
		// 加载配置文件
		bool loadConfig(std::string path="config.json");

		// 根据配置文件连接服务器
		int tcpConnect();

	private:
		std::string server_addr_;
		std::string server_port_;
};


