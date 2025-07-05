#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig) {
    printf("Caught signal %d (SIGINT). Exiting cleanly...\n", sig);
//    _exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);  // Register handler

    while (1) {
        printf("Running... Press Ctrl+C to stop.\n");
        sleep(1);
    }

    return 0;
}

