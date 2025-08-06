#include <stdio.h>              // For printf(), scanf(), perror()
#include <stdlib.h>             // For general functions like exit()
#include <fcntl.h>              // For open() and file control options like O_RDWR
#include <unistd.h>             // For close(), read(), write()
#include <sys/ioctl.h>          // For ioctl() system call
#include <string.h>             // For string functions like strlen, strcpy (if used)

#define DEVICE_FILE "/dev/x86_uart"  // Device file path for UART communication with driver

// ioctl command definitions
#define IOCTL_SEND_PASSWORD    _IOW('x', 1, char *) // Send password (write operation)
#define IOCTL_SEND_THRESHOLD   _IOW('x', 2, int)    // Send threshold temperature (write operation)

// Function to send password and threshold to the kernel driver
void send_password_and_threshold(int fd) {
    char password[32];  // Buffer to store password input
    int threshold;      // Variable to store temperature threshold

    // Prompt user for password
    printf("Enter new password to send to RPi: ");
    scanf("%31s", password);  // Read up to 31 characters (leaves space for null-terminator)

    // Prompt user for threshold value
    printf("Enter new threshold value (°C): ");
    scanf("%d", &threshold);

    // Send password using ioctl
    if (ioctl(fd, IOCTL_SEND_PASSWORD, password) < 0) {
        perror("IOCTL_SEND_PASSWORD failed");  // Print error if ioctl fails
        return;
    }

    // Send threshold using ioctl
    if (ioctl(fd, IOCTL_SEND_THRESHOLD, &threshold) < 0) {
        perror("IOCTL_SEND_THRESHOLD failed");  // Print error if ioctl fails
        return;
    }

    // Confirmation message
    printf("Password and threshold sent successfully to Raspberry Pi via UART.\n");
}

int main() {
    int fd, choice;

    // Open the device file for reading and writing
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device file");  // Print error and exit if open fails
        return 1;
    }

    // Main menu loop
    while (1) {
        // Display menu to user
        printf("\n--- x86_1 User Menu ---\n");
        printf("1. Set initial password & threshold\n");
        printf("2. Update password & threshold\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);  // Read user choice

        // Process user's choice
        switch (choice) {
            case 1:
            case 2:
                send_password_and_threshold(fd);  // Call function to send data to driver
                break;
            case 3:
                close(fd);  // Close the device file before exiting
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");  // Handle invalid input
        }
    }
}
