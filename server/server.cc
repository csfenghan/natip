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

void send_image(int client_fd, VideoCapture &cap) {
        Mat image;
        vector<uchar> data_encode;

        while (cap.read(image)) {
                if (!imencode(".jpg", image, data_encode)) {
                        cerr << "imencode error" << endl;
                        exit(0);
                }
                // set header size
                string data_len = to_string(data_encode.size());
		int n=16-data_len.size();
                for (int i = 0; i < n; i++) {
                        data_len += " ";
                }
                send(client_fd, data_len.c_str(), strlen(data_len.c_str()), 0);

                // sending data
                string data_send(data_encode.begin(), data_encode.end());
                send(client_fd, data_send.c_str(), data_send.size(), 0);
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
        server_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, ADDR, &server_addr.sin_addr);

        bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        listen(server_fd, 10);

        while (1) {
                client_fd = accept(server_fd, NULL, NULL);
                printf("accept successful\n");
                if (fork() == 0) {
                        close(server_fd);
                        send_image(client_fd, cap);
                        exit(0);
                }
                close(client_fd);
        }
}
