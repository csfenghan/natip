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

void echo_cli(int sockfd) {
        char buf[1024];

        while (1) {
                bzero(buf, sizeof(buf));
                printf(">");
                fflush(stdout);
                fgets(buf, sizeof(buf) - 1, stdin);
                send(sockfd, buf, strlen(buf), 0);

                bzero(buf, sizeof(buf));
                recv(sockfd, buf, sizeof(buf) - 1, 0);
                printf("%s", buf);
                buf[strlen(buf) - 1] = '\0';
                if (strcmp(buf, "exit") == 0)
                        break;
        }
}

int main(int argc, char **argv) {
        int sockfd, sockfd2;
        SA_IN server;
        IP ip;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        bzero(&server, sizeof(SA_IN));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(SER);
        server.sin_port = htons(PORT);

        connect(sockfd, (SA *)&server, sizeof(SA_IN));
        recv(sockfd, &ip, sizeof(IP), 0);
        close(sockfd);

        sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_addr = ip.ip;
        server.sin_port = ip.port;
        while (connect(sockfd2, (SA *)&server, sizeof(SA_IN)) == -1)
                perror("connect");

        echo_cli(sockfd2);

        close(sockfd2);
        return 0;
}
