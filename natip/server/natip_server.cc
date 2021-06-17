#include "natip_server.hpp"
#include "unix_api.h"
#include <jsoncpp/json/json.h>
#include <fstream>

bool NatIpServer::loadConfig(std::string path) {
	Json::Value root, server;
	Json::Reader reader;
	std::ifstream in_file_stream;

	// 1.读取config文件
	in_file_stream.open(path, std::ifstream::in);
	if (!in_file_stream.is_open()){
		fprintf(stderr,"can't open file: %s", path.c_str());
	}

	if (!reader.parse(in_file_stream, root))
		fprintf(stderr,"can't parse file: %s", path.c_str());

	// 2.使用config文件进行配置
	if (!root.isMember("server"))
		fprintf(stderr,"can't found key: 'server' in config file %s", path.c_str());
	server = root["server"];

	if (!server.isMember("listen_port"))
		fprintf(stderr,"can't found key: 'listen_port' in object 'server' in config file %s",
				path.c_str());

	listen_port_= server["listen_port"].asString();

	return true;
}

void NatIpServer::tcpAccept(){
	pid_t pid;
	int listenfd,client_fd;
	struct sockaddr_storage client_addr;
	socklen_t client_len;

	if((listenfd=tcpListen())<0){
		perror("get listenfd failed");
		return;
	}

	// 等待连接，处理连接
	while(true){
		client_fd=Accept(listenfd,(SA *)&client_addr,&client_len);
		if((pid=Fork())==0){
			close(listenfd);
			printf("child process\n");
			exit(0);
		}
		close(client_fd);
	}
}

int NatIpServer::tcpListen(){
	struct  addrinfo hints,*listp,*p;
	int listenfd;

	// 1. 获取监听的服务器地址和端口
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
	Getaddrinfo(NULL,listen_port_.c_str(),&hints,&listp);

	// 2. bind描述符
	for(p=listp;p;p=listp->ai_next){
		if((listenfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))<0)
			continue;

		if(bind(listenfd,p->ai_addr,p->ai_addrlen)==0)
			break;
		Close(listenfd);
	}

	// 3.listen描述符
	freeaddrinfo(listp);
	if(!p)
		return -1;
	if(listen(listenfd,10)<0){
		Close(listenfd);
		return -1;
	}

	return listenfd;
}
