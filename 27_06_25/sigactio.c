#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig) {
    printf("Caught signal %d (SIGINT). Exiting gracefully!\n", sig);
    _exit(0);
}

int main() {
    struct sigaction sa;

    sa.sa_handler = handle_sigint;     // Set the handler function
    sigemptyset(&sa.sa_mask);          // Don't block any other signals
    sa.sa_flags = 0;                   // No special flags

    sigaction(SIGINT, &sa, NULL);      // Install handler for SIGINT

    while (1) {
        printf("Running... Press Ctrl+C to send SIGINT\n");
        sleep(1);
    }

    return 0;
}

