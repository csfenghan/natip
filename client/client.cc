#include <arpa/inet.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <sys/socket.h>
#include <vector>

using namespace std;
using namespace cv;

#define ADDR "127.0.0.1"
#define PORT 11111

void receive_videos(int client_fd, char *output) {
        Mat image_decode;
        vector<uchar> data;
        char recv_buf[16];

        while (1) {
                if (recv(client_fd, recv_buf, 16, 0)) {
                        for (int i = 0; i < 16; i++)
                                if (recv_buf[i] < '0' || recv_buf[i] > '9')
                                        recv_buf[i] = ' ';
                        int len = atoi(recv_buf);
                        memset(recv_buf, 0, 16);
                        data.resize(len);

                        for (int i = 0; i < len; i++) {
                                recv(client_fd, recv_buf, 1, 0);
                                data[i] = recv_buf[0];
                        }

                        image_decode = imdecode(data, CV_LOAD_IMAGE_COLOR);
                        if (image_decode.empty()) {
                                cerr << "imdecode error" << endl;
                                exit(0);
                        }
                        imshow("client", image_decode);
			waitKey(30);
                }
        }
}

int main(int argc, char **argv) {
        if (argc != 2) {
                fprintf(stderr, "usage: %s video.file\n", argv[0]);
                //         exit(0);
        }
        int client_fd;
        struct sockaddr_in server_addr;

        client_fd = socket(AF_INET, SOCK_STREAM, 0);

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, ADDR, &server_addr.sin_addr);
        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
                cerr << "connect failed" << endl;
                exit(0);
        }

        cout << "connection successful" << endl;
        receive_videos(client_fd, argv[1]);
        cout << "successful video reception" << endl;
}
