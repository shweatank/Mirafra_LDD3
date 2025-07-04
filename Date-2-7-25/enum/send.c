#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

enum color {
    RED = 1,
    GREEN,
    BLUE
};
int main() 
{
    int fd;

    enum color c = GREEN;

    fd = open("/dev/simple_char_dev", O_RDWR);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }

    write(fd, &c , sizeof(c));

    close(fd);
    return 0;
}

