// File: signal_sender.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include<string.h>
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <pid> \n", argv[0]);
        printf("Signals: int → SIGINT, quit → SIGQUIT\n");
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    int sig;

    if (strcmp(argv[2], "int") == 0)
        sig = SIGINT;
    else if (strcmp(argv[2], "quit") == 0)
        sig = SIGQUIT;
    else {
        printf("Unknown signal\n");
        return 1;
    }

    if (kill(pid, sig) == 0)
        printf("Signal %s sent to PID %d\n", argv[2], pid);
    else
        perror("kill");

    return 0;
}

