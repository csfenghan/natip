#include "natip_server.hpp"

int main(int argc,char **argv){
	NatIpServer server;

	// 1.加载配置
	server.loadConfig("config.json");

	// 2. 初始化
	server.tcpInit();

	// 3.服务器主循环
	server.tcpServer();
}
