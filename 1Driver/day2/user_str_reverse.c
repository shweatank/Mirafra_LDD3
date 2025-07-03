// user_reverse.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/reverser"
#define BUF_SIZE 1024

int main() {
    int fd;
    char write_buf[BUF_SIZE];
    char read_buf[BUF_SIZE];
    ssize_t bytes_written, bytes_read;

    // Open the device
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return 1;
    }

    // Input string to send
    printf("Enter a string to send to kernel: ");
    fgets(write_buf, BUF_SIZE, stdin);

    // Remove newline
    size_t len = strlen(write_buf);
    if (len > 0 && write_buf[len - 1] == '\n') {
        write_buf[len - 1] = '\0';
    }

    // Write string to device
    bytes_written = write(fd, write_buf, strlen(write_buf));
    if (bytes_written < 0) {
        perror("Failed to write to the device");
        close(fd);
        return 1;
    }

    // Read the reversed string from device
    bytes_read = read(fd, read_buf, BUF_SIZE);
    if (bytes_read < 0) {
        perror("Failed to read from the device");
        close(fd);
        return 1;
    }

    read_buf[bytes_read] = '\0'; // Null terminate
    printf("Reversed string from kernel: %s\n", read_buf);

    close(fd);
    return 0;
}

