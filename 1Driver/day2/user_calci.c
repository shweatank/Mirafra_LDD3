#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/calculator"

int main() {
    int fd;
    char input[100];
    char output[100];

    // Open the device
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Get input from user
    int op1, op2;
    char op;

    printf("Enter first number: ");
    scanf("%d", &op1);

    printf("Enter second number: ");
    scanf("%d", &op2);

    printf("Enter operator (+, -, *, /): ");
    scanf(" %c", &op);  // note space before %c to skip newline

    // Format input string as "10 5 *"
    snprintf(input, sizeof(input), "%d %d %c", op1, op2, op);

    // Write input to kernel
    if (write(fd, input, strlen(input)) < 0) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    // Read result from kernel
    if (read(fd, output, sizeof(output)) < 0) {
        perror("Read failed");
        close(fd);
        return 1;
    }

    // Print result
    printf("Result from kernel: %s", output);

    close(fd);
    return 0;
}

