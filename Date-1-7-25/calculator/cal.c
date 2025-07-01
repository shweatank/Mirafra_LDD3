#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int main() 
{
    int fd;
    char write_buf[100], read_buf[100];

    printf("Enter a operation to send to kernel: ");
    scanf(" %s",write_buf);


    fd = open("/dev/simple_char_dev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    write(fd, write_buf, strlen(write_buf));

    lseek(fd, 0, SEEK_SET);

    read(fd, read_buf, sizeof(read_buf));

    printf("Result from kernel: %s\n", read_buf);

    close(fd);
    return 0;
}

