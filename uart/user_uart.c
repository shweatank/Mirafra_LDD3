#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// Simple string reversal
void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}

int main() {
    int fd;
    char read_buf[256], write_buf[256];
    ssize_t bytes_read;

    // Open the device for read and write in non-blocking mode
    fd = open("/dev/my_uart", O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    while (1) {
        // Try to read from UART
        bytes_read = read(fd, read_buf, sizeof(read_buf) - 1);
        if (bytes_read < 0) {
            if (errno == EAGAIN) {
                // No data available yet
                usleep(100000);  // 100 ms sleep
                continue;
            } else {
                perror("read");
                break;
            }
        } else if (bytes_read == 0) {
            // No data
            usleep(100000);
            continue;
        }

        // Null-terminate and print received data
        read_buf[bytes_read] = '\0';
        printf("Received: %s\n", read_buf);

        // Process the data (e.g., reverse it)
        strncpy(write_buf, read_buf, sizeof(write_buf));
        reverse_string(write_buf);
        printf("Processed: %s\n", write_buf);

        // Write processed data back
        ssize_t bytes_written = write(fd, write_buf, strlen(write_buf));
        if (bytes_written < 0) {
            perror("write");
        }

        // Clear buffers for next iteration
        memset(read_buf, 0, sizeof(read_buf));
        memset(write_buf, 0, sizeof(write_buf));
    }

    close(fd);
    return 0;
}

