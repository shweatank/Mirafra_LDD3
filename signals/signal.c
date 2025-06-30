#include <stdio.h>
#include <signal.h>
#include <stdlib.h> // Required for exit()
#include <unistd.h> // Required for sleep()

// Global flag to control the main loop
volatile sig_atomic_t keep_running = 1;

// Signal handler for SIGINT
void sigint_handler(int signum) {
    printf("\nCaught SIGINT (signal %d)! Cleaning up and exiting...\n", signum);
}

int main() {
    // Register the signal handler for SIGINT
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error setting up signal handler");
        return 1; // Indicate an error
    }

    printf("Program started. Press Ctrl+C to send SIGINT and exit.\n");

    // Continuous loop in main
    while (keep_running) {
        printf("Main loop is running...\n");
        sleep(1); // Sleep for 1 second to avoid excessive CPU usage
    }

    printf("Main loop terminated. Program exiting gracefully.\n");
    return 0; // Indicate successful execution
}
