#include <unix_api.h>

const char server[] = "99809412876dcc63.natapp.cc";
const short port = 13650;

int main(int argc, char **argv) {
        int sockfd;
	struct addrinfo *res;

	Getaddrinfo(server,NULL,NULL,&res);	
	sockfd=Socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	Connect(sockfd,res->ai_addr,res->ai_addrlen);

	freeaddrinfo(res);
        exit(0);
}
