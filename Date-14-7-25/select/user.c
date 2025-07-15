#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>

int main() 

{
    int fd = open("/dev/poll_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    printf("Waiting for data using select...\n");
    retval = select(fd + 1, &rfds, NULL, NULL, NULL);
    if (retval == -1) {
        perror("select");
    } else if (retval) {
        char buf[128] = {0};
        read(fd, buf, sizeof(buf));
        printf("Received: %s\n", buf);
    } else {
        printf("Timeout! No data.\n");
    }

    close(fd);
    return 0;
}
