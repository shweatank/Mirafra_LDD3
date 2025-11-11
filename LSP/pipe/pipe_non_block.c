#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    int fd[2];
    char buf[50];
    char msg[] = "hello non-blocking pipe";

    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    // set read end of pipe to NON-BLOCKING
    int flags = fcntl(fd[0], F_GETFL, 0);
    fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid > 0) { // Parent
        close(fd[0]); // close read end
        sleep(2); // simulate delay
        write(fd[1], msg, strlen(msg) + 1);
        printf("Parent: written message\n");
        close(fd[1]);
    } else { // Child
        close(fd[1]); // close write end
        int ret;

        // first attempt: should fail because no data yet
        ret = read(fd[0], buf, sizeof(buf));
        if (ret == -1 && errno == EAGAIN)
            printf("Child: no data available yet (non-blocking)\n");

        sleep(3); // wait for parent to write

        // second attempt: now data should be there
        ret = read(fd[0], buf, sizeof(buf));
        if (ret > 0)
            printf("Child: read message: %s\n", buf);

        close(fd[0]);
    }

    return 0;
}

