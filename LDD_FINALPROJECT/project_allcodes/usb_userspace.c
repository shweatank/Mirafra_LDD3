#include <stdio.h>      // Standard I/O functions (e.g., fopen, printf)
#include <stdlib.h>     // Standard library functions (e.g., exit)
#include <unistd.h>     // POSIX functions (e.g., sleep)
#include <string.h>     // String manipulation functions (included but not used here)

#define PROC_PATH "/proc/usb_detect"  // Path to the virtual proc file created by the kernel module

int main() {
    FILE *fp;                      // File pointer to read from /proc/usb_detect
    char buffer[1024];             // Buffer to store each line read from the proc file

    while (1) {                    // Infinite loop to continuously monitor USB detection
        fp = fopen(PROC_PATH, "r");  // Open the proc file in read mode
        if (fp == NULL) {             // Check if file opened successfully
            perror("Failed to open /proc/usb_detect");  // Print error if open fails
            return 1;                                   // Exit the program with error code
        }

        printf("USB Waiting For Connection...\n");  // Print status message
        while (fgets(buffer, sizeof(buffer), fp)) {  // Read lines from the proc file
            printf("%s", buffer);                     // Display the content (includes newline)
            sleep(5);                                 // Wait for 5 seconds for user to read
            exit(0);                                  // Exit after displaying first message
        }
        fclose(fp);            // Close the file to free system resources

        sleep(2);              // Wait before checking again to reduce CPU usage
        // system("clear");   // Optional: Uncomment to clear the terminal each loop
    }

    return 0;   // Program should not reach here due to infinite loop
}

