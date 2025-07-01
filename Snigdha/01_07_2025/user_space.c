#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/simple_char_dev"

int main() 
{
    int fd;
    char read_buf[256],write_buf[256];
    int number;

    printf("Enter any number:");
    scanf("%d",&number);

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }

    snprintf(write_buf, sizeof(write_buf),"%d",number);

    if (write(fd, write_buf, strlen(write_buf)) < 0) 
    {
        perror("Write failed");
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET);

    int bytes = read(fd, read_buf, sizeof(read_buf) - 1);
    if (bytes < 0) 
    {
        perror("Read failed");
        close(fd);
        return 1;
    }

    read_buf[bytes] = '\0';
    printf("Incremented number from kernel : %s\n", read_buf);

    close(fd);
    return 0;
}

