#include <unix_api.h>

const char server[] = "99809412876dcc63.natapp.cc";
const char port[] = "13650";

int main(int argc, char **argv) {
        int sockfd;
        struct addrinfo *res;

        Getaddrinfo(server, port, NULL, &res);
        // do {
        sockfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        Connect(sockfd, res->ai_addr, res->ai_addrlen);
        //} while ((res = res->ai_next) != NULL);

        freeaddrinfo(res);
        exit(0);
}
