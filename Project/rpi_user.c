// Standard input/output
#include <stdio.h>

// For exit(), atoi(), etc.
#include <stdlib.h>

// For string manipulation (e.g., sscanf, strcpy)
#include <string.h>

// For open() flags
#include <fcntl.h>

// For close(), read(), etc.
#include <unistd.h>

// For ioctl system call
#include <sys/ioctl.h>

// For errno values like EPERM
#include <errno.h>

// Device file created by your kernel module
#define DEVICE_FILE "/dev/rpi_secure"

// UART device file (connected to external input)
#define UART_DEVICE "/dev/serial0"

// ioctl command macros for communication with kernel
#define IOCTL_SEND_PASSWORD     _IOW('x', 1, char *)  // Send password to kernel
#define IOCTL_VERIFY_PASSWORD   _IOW('x', 2, char *)  // Verify user password
#define IOCTL_SEND_THRESHOLD    _IOW('x', 3, int)     // Send threshold after password check

// Function to read password and threshold from UART (sent by x86 board)
void read_uart_config(char *password, int *threshold) {
    int uart_fd;
    char uart_buf[128];

    // Open UART device in read-only mode
    uart_fd = open(UART_DEVICE, O_RDONLY);
    if (uart_fd < 0) {
        perror("Failed to open UART device");
        exit(EXIT_FAILURE);
    }

    // Read from UART
    int len = read(uart_fd, uart_buf, sizeof(uart_buf) - 1);
    close(uart_fd);

    if (len <= 0) {
        perror("Failed to read from UART");
        exit(EXIT_FAILURE);
    }

    // Null-terminate the read string
    uart_buf[len] = '\0';

    // Expected format: PASS:<password> THRESH:<value>
    if (sscanf(uart_buf, "PASS:%31s THRESH:%d", password, threshold) != 2) {
        fprintf(stderr, "UART format invalid. Expected: PASS:<password> THRESH:<value>\n");
        exit(EXIT_FAILURE);
    }

    printf("Received via UART:\n  Password: %s\n  Threshold: %d\n", password, *threshold);
}

// Function to send password to kernel using ioctl
int send_password_to_kernel(int dev_fd, const char *password) {
    if (ioctl(dev_fd, IOCTL_SEND_PASSWORD, password) < 0) {
        perror("IOCTL_SEND_PASSWORD failed");
        return -1;
    }
    return 0;
}

// Function to manually verify password by prompting user
int verify_password(int dev_fd) {
    char input_pass[32];

    printf("Enter password to verify: ");
    scanf("%31s", input_pass);  // Read user input

    // Send password to kernel via ioctl
    if (ioctl(dev_fd, IOCTL_VERIFY_PASSWORD, input_pass) == 0) {
        printf("Password verified successfully.\n");
        return 0;
    } else if (errno == EPERM) {
        printf("Password incorrect.\n");
        return -1;
    } else {
        perror("IOCTL_VERIFY_PASSWORD failed");
        return -1;
    }
}

// Function to send threshold value to kernel for monitoring
int send_threshold_to_kernel(int dev_fd, int threshold) {
    if (ioctl(dev_fd, IOCTL_SEND_THRESHOLD, &threshold) < 0) {
        if (errno == EPERM)
            fprintf(stderr, "Must verify password before sending threshold.\n");
        else
            perror("IOCTL_SEND_THRESHOLD failed");
        return -1;
    }

    printf("Threshold %d sent. Monitoring started.\n", threshold);
    return 0;
}

// Main function: menu-driven interface
int main() {
    int dev_fd;
    char password[32] = "";   // Buffer for password
    int threshold = 0, choice;

    // Open kernel module device file
    dev_fd = open(DEVICE_FILE, O_RDWR);
    if (dev_fd < 0) {
        perror("Failed to open /dev/rpi_secure");
        return EXIT_FAILURE;
    }

    // Infinite loop for menu options
    while (1) {
        printf("\n--- Raspberry Pi Secure Monitor ---\n");
        printf("1. Read config from UART and send to kernel\n");
        printf("2. Verify password manually\n");
        printf("3. Send threshold (after password verified)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        // Read user input for choice
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input.\n");
            while (getchar() != '\n'); // Clear stdin buffer
            continue;
        }

        // Handle menu options
        switch (choice) {
        case 1:
            // Read password and threshold from UART, send to kernel
            read_uart_config(password, &threshold);
            if (send_password_to_kernel(dev_fd, password) == 0)
                printf("Password sent to kernel.\n");
            break;

        case 2:
            // Manual password verification
            verify_password(dev_fd);
            break;

        case 3:
            // Send threshold if password and value exist
            if (threshold == 0) {
                printf("Threshold not set. Run option 1 first.\n");
                break;
            }
            send_threshold_to_kernel(dev_fd, threshold);
            break;

        case 4:
            // Exit the program
            close(dev_fd);
            printf("Exiting...\n");
            return 0;

        default:
            printf("Invalid choice. Try again.\n");
        }
    }

    // Close device file (technically unreachable due to return above)
    close(dev_fd);
    return 0;
}

