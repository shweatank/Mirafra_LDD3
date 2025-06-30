#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

pid_t p2_pid;

void handle_sigint(int sig) {
    printf("\n[ P1] Caught SIGINT (Ctrl+C)\n");
    kill(p2_pid, SIGUSR1); // Send signal to P2
}

void handle_sigtstp(int sig) {
    printf("\n[ P1] Caught SIGTSTP (Ctrl+Z)\n");
    kill(p2_pid, SIGUSR2); // Send different signal to P2
}

int main() {
    printf("[P1] Enter PID of P2: ");
    scanf("%d", &p2_pid);

    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);

    while (1) {
        pause(); // Wait for signal
    }

    return 0;
}

