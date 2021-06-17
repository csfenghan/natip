#include "natip_server.hpp"

int main(int argc,char **argv){
	NatIpServer server;

	if(!server.loadConfig("config.json")){
		fprintf(stderr,"load config file error!\n");
		return -1;
	}
	server.processConnect();
}
