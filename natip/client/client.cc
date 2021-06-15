#include <unix_api.h>

int main(int argc, char **argv) {
        int sockfd;
        struct sockaddr_in server_addr;

        sockfd = Socket(AF_INET, SOCK_STREAM, 0);

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_port = htons(13650);
        server_addr.sin_family = AF_INET;
        Connect(sockfd, (SA *)&server_addr, sizeof(server_addr));

        exit(0);
}
