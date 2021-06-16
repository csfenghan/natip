#include <unix_api.h>

const char server[] = "1.117.67.108";
const short port = 8888;

int main(int argc, char **argv) {
        int sockfd;
        struct sockaddr_in addr;

        sockfd = Socket(AF_INET, SOCK_STREAM, 0);

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, server, &addr.sin_addr);
        Connect(sockfd, (SA *)&addr, sizeof(addr));

        exit(0);
}
