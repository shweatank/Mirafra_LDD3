#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid = vfork();

    if (pid < 0) {
        perror("vfork failed");
        return 1;
    }

    if (pid == 0) {
        printf("Child (vfork): PID = %d\n", getpid());
        _exit(0);  // use _exit() instead of exit()
    } else {
        printf("Parent (after vfork): PID = %d\n", getpid());
    }

    return 0;
}

