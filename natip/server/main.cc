#include "natip_server.hpp"

int main(int argc,char **argv){
	NatIpServer server;

	// 1. 初始化
	server.init();

	// 2.加载配置
	server.loadConfig("config.json");

	// 3.服务器主循环
	server.tcpServer();
}
