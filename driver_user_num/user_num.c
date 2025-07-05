#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/simple_char_num1"

int main()
{
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int input = 42;
    ssize_t written = write(fd, &input, sizeof(int));
    printf("Sent: %d (written %zd bytes)\n", input, written);

    int output = 0;
    ssize_t read_bytes = read(fd, &output, sizeof(int));
    printf("Received: %d (read %zd bytes)\n", output, read_bytes);

    close(fd);
    return 0;
}

