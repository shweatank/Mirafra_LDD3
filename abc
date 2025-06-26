#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handler(int sig) {
    printf("Child received signal %d\n", sig);
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        signal(SIGUSR1, handler);
        pause();  // wait for signal
    } else {
        sleep(1);
        kill(pid, SIGUSR1);  // parent sends signal to child
        wait(NULL);
    }
    return 0;
	//pavan
}
