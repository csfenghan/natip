#include <arpa/inet.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;
using namespace cv;

#define ADDR "127.0.0.1"
#define PORT 8888

void sent_image(int client_fd, VideoCapture &cap) {
        Mat image;
        vector<unsigned char> data_encode;
        string data_send;
        char buf[1];

        while (cap.read(image)) {
                imencode(".jpg", image, data_encode);

                // set header size
                string data_len = to_string(data_encode.size());
                for (int i = 0; i < 16 - data_len.size(); i++) {
                        data_len += " ";
                }

                // sending data
                send(client_fd, data_len.c_str(), strlen(data_len.c_str()), 0);
                for (int i = 0; i < data_encode.size(); i++) {
                        buf[0] = data_send[i];
                        send(client_fd, buf, 1, 0);
                }

                // receive the return information
                char buf_recv[32];
                recv(client_fd, buf_recv, 32, 0);
                printf("recvived information:%s\n", buf_recv);
        }
}

int main(int argc, char **argv) {
        if (argc != 2) {
                fprintf(stderr, "usage: %s video.file\n", argv[0]);
                exit(0);
        }

        int server_fd, client_fd;
        struct sockaddr_in server_addr;
        VideoCapture cap;

        // read the videos
        cap.open(argv[1]);
        if (!cap.isOpened()) {
                cerr << "can't open videos" << endl;
                exit(0);
        }

        // setting up the server
        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8888);
        inet_pton(AF_INET, ADDR, &server_addr.sin_addr);

        bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        listen(server_fd, 10);

        while (1) {
                client_fd = accept(server_fd, NULL, NULL);
                if (fork() == 0) {
                        close(server_fd);
                        sent_image(client_fd, cap);
                        exit(0);
                }
                close(client_fd);
        }
}
