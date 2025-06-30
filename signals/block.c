#include <stdio.h>
#include <signal.h>
#include <stdlib.h> // Required for exit()
#include <unistd.h> // Required for sleep()


void sigint_handler(int signum) {
    printf("\nCaught SIGINT (signal %d)! Cleaning up and exiting...\n", signum);
}

int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error setting up signal handler");
        return 1; // Indicate an error
    }

    sigset_t block_set;

    sigemptyset(&block_set);

    sigaddset(&block_set, SIGINT);

    printf("Program started. Press Ctrl+C to send SIGINT and exit.\n");
    printf("Blocking SIGINT for 5 sec....\n");
    sigprocmask(SIG_BLOCK, &block_set, NULL);
    sleep(5);


    printf("Blocking SIGINT for 5 sec....\n");
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);
    // Continuous loop in main
    while (1) {
    	pause();
    }

    printf("Main loop terminated. Program exiting gracefully.\n");
    return 0; // Indicate successful execution
}
