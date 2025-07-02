#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_NAME "sample_char_sim.txt"
#define BUF_LEN 128

char device_buffer[BUF_LEN];

int dev_open() {
    printf("simple_char_dev: Device opened\n");
    return open(DEVICE_NAME, O_RDWR | O_CREAT, 0666);
}

int dev_release(int fd) {
    printf("simple_char_dev: Device closed\n");
    return close(fd);
}

ssize_t dev_read(int fd, char *user_buf) {
    lseek(fd, 0, SEEK_SET);  // rewind
    int bytes_read = read(fd, user_buf, BUF_LEN);
    user_buf[bytes_read] = '\0'; // null-terminate
    printf("simple_char_dev: Read %d bytes: %s\n", bytes_read, user_buf);
    return bytes_read;
}

ssize_t dev_write(int fd, const char *user_buf) {
    int len = strlen(user_buf);
    if (len > BUF_LEN - 1)
        len = BUF_LEN - 1;

    strncpy(device_buffer, user_buf, len);
    device_buffer[len] = '\0';

    lseek(fd, 0, SEEK_SET);  // overwrite from beginning
    write(fd, device_buffer, strlen(device_buffer));
    printf("simple_char_dev: Written %d bytes: %s\n", len, device_buffer);
    return len;
}

int main() {
    int fd = dev_open();
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    const char *msg = "Hello from user space!";
    dev_write(fd, msg);

    char read_buf[BUF_LEN];
    dev_read(fd, read_buf);

    dev_release(fd);
    return 0;
}

