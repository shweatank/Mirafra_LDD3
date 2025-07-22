#include <poll.h>
#include <stdio.h>

int main() {
    struct pollfd fds[2];  // Array of 2 pollfd structs

    // Monitor STDIN
    fds[0].fd = 0;           // STDIN
    fds[0].events = POLLIN; // Wait for input

    // Monitor a socket
    fds[1].fd = 4;           // Some socket FD
    fds[1].events = POLLIN; // Wait for data to read

    int nfds = 2;            // Total number of FDs to monitor

    int ret = poll(fds, nfds, 5000);  // 5-second timeout

    if (ret > 0) {
        if (fds[0].revents & POLLIN) {
            printf("STDIN is ready for reading\n");
        }
        if (fds[1].revents & POLLIN) {
            printf("Socket is ready for reading\n");
        }
    } else if (ret == 0) {
        printf("Timeout: No input\n");
    } else {
        perror("poll");
    }

    return 0;
}

