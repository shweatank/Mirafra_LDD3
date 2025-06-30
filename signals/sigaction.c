#include <stdio.h>
#include <signal.h>
#include <unistd.h> // For sleep()

// Signal handler function for SIGINT
void sigint_handler(int signum) {
    printf("\nCaught SIGINT (Ctrl+C)! Printing some data.\n");
    // You can add more logic or data printing here
}

int main() {
    struct sigaction sa;

    // Initialize the sigaction struct
    sa.sa_handler = sigint_handler; // Set the signal handler function
    sigemptyset(&sa.sa_mask);      // Clear the signal mask (no signals blocked during handler execution)
    sa.sa_flags = 0;               // No special flags

    // Register the signal handler for SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error setting up signal handler for SIGINT");
        return 1;
    }

    printf("Program running. Press Ctrl+C to trigger the SIGINT handler.\n");

    // Continuous while loop in main
    while (1) {
        printf("Main loop: Doing some work...\n");
        sleep(2); // Simulate some work
    }

    return 0; // This line will not be reached in this example
}
