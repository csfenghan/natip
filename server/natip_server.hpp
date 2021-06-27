#ifndef NATIP_SERVER_HPP
#define NATIP_SERVER_HPP

#include "jsoncpp/json/json.h"
#include "jdt_connection.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace natip {

class NatIpServer {
      public:
	// 构造函数
	NatIpServer();	

	// 析构函数
	~NatIpServer();

        // 加载配置文件
        void loadConfig(std::string path);

	// 开始listen
	void startListen();

	// 停止listen
	void stopListen();

	// accept一个连接，返回一个消息节点
	jdt::Connection acceptConnection();	

      private:
        // 信号处理函数
        static void sigchldHandler(int sig);
	static void sigintHandler(int sig);

        int listen_fd_;           // 服务器的listen描述符
        std::string listen_port_; // 服务器监听的端口
};

} // namespace natip
#endif
