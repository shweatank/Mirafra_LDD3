#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/simple_char"

int main()
{
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open device");
        return 1;
    }
    char write_buf[] = "hello";
    ssize_t written = write(fd, write_buf, strlen(write_buf));
    if (written < 0)
    {
        perror("Failed to write to device");
        close(fd);
        return 1;
    }
    printf("Written %zd bytes: %s\n", written, write_buf);
    char read_buf[128];
    ssize_t read_bytes = read(fd, read_buf, sizeof(read_buf)-1);
    if (read_bytes < 0)
    {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }
    read_buf[read_bytes] = '\0';
    printf("Read %zd bytes: %s\n", read_bytes, read_buf);
    close(fd);
    return 0;
}
