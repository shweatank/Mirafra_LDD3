#include <stdio.h>
#include <poll.h>
#include <unistd.h>

int main() {
    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    printf("Waiting for input (5 seconds)...\n");
    int ret = poll(fds, 1, 5000);

    if (ret > 0 && (fds[0].revents & POLLIN)) {
        char buf[100];
        read(STDIN_FILENO, buf, sizeof(buf));
        printf("You typed: %s", buf);
    } else if (ret == 0) {
        printf("Timeout: No input.\n");
    } else {
        perror("poll");
    }

    return 0;
}

