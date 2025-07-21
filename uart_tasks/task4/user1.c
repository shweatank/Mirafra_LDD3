#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEV_PATH "/dev/keylog"

int main() {
    int fd;
    char buf[128];
    ssize_t n;

    fd = open(DEV_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Listening for strings from kernel (Press Ctrl+C to stop)...\n");

    while (1) {
        n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received: %s\n", buf);
        } else if (n == 0) {
            continue; // Nothing yet
        } else {
            perror("read");
            break;
        }
    }

    close(fd);
    return 0;
}

