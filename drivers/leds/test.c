#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
        int fd, val;

        if (argc != 2) {
                fprintf(stderr, "usage: %s on/off\n", argv[0]);
                exit(0);
        }

        if ((fd = open("/dev/led1", O_RDWR)) < 0) {
                fprintf(stderr, "open /dev/led error\n");
                exit(0);
        }

        if (strcmp(argv[1], "on") == 0) {
                val = 1;
                write(fd, &val, 4);
        } else if (strcmp(argv[1], "off") == 0) {
                val = 0;
                write(fd, &val, 4);
        }

        exit(0);
}
