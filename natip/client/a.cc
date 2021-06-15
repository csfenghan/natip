#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SER "xxx.xxx.xxx.xxx"
#define PORT 8888

typedef struct {
        struct in_addr ip;
        int port;
} IP; // ip与端口

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_IN;

//回射服务
void echo_ser(int sockfd) {
        char buf[1024];

        while (1) {
                bzero(buf, sizeof(buf));
                //接收B发来的数据
                recv(sockfd, buf, sizeof(buf) - 1, 0);
                printf("%s", buf);
                //向B发送数据
                send(sockfd, buf, strlen(buf), 0);

                buf[strlen(buf) - 1] = '\0';
                if (strcmp(buf, "exit") == 0)
                        break;
        }
}

int main(int argc, char **argv) {
        int sockfd, sockfd2;
        SA_IN server, addr;
        IP ip;
        socklen_t addrlen = sizeof(SA_IN);
        char s = 'a';
        int flags = 1;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        bzero(&server, sizeof(SA_IN));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(SER);
        server.sin_port = htons(PORT);

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(int)) == -1)
                perror("setsockopt sockfd");
        connect(sockfd, (SA *)&server, sizeof(SA_IN));
        send(sockfd, &s, sizeof(char), 0);
        recv(sockfd, &ip, sizeof(IP), 0);
        close(sockfd);

        sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd2 == -1)
                perror("sockfd2");
        if (setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(int)) == -1)
                perror("setsockopt sockfd2");
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = ip.port;
        if (bind(sockfd2, (SA *)&server, sizeof(SA_IN)) == -1)
                perror("bind sockfd");
        if (listen(sockfd2, 20) == -1)
                perror("listen");

        echo_ser(accept(sockfd2, (SA *)&addr, &addrlen));

        close(sockfd2);

        return 0;
}
