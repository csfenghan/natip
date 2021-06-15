#include <unix_api.h>

int main(int argc, char **argv) {
        int listenfd, connfd;
        pid_t child_pid;
        socklen_t client_len;
        struct sockaddr_in client_addr, server_addr;

        listenfd = Socket(AF_INET, SOCK_STREAM, 0);

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(8888);
        Bind(listenfd, (SA *)&server_addr, sizeof(server_addr));

        Listen(listenfd, 5);

        while (1) {
                client_len = sizeof(client_addr);
                connfd = Accept(listenfd, (SA *)&client_addr, &client_len);
                if ((child_pid = Fork()) == 0) {
			Close(listenfd);

			exit(0);
                }
		Close(connfd);
        }
}
