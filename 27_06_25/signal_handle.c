#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    pid_t pid = 1234; // Replace with the target process ID
    int result = kill(pid, SIGINT);  // Send SIGINT

    if (result == 0) {
        printf("Signal sent successfully.\n");
    } else {
        perror("Error sending signal");
    }

    return 0;
}

