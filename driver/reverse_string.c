#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int fd = open("/dev/simple_char_dev", O_RDWR);
    if (fd < 0)
    {
        perror("fd error");
        return 1;
    }

    write(fd, "heyy Hii", 8);
    printf("Data written\n");

    lseek(fd, 0, SEEK_SET);

    char buf[32];
    int n = read(fd, buf, 8);
    buf[n] = '\0';

      for (int i = 0; i < n / 2; i++)
     {
        char temp = buf[i];
        buf[i] = buf[n - 1 - i];
        buf[n - 1 - i] = temp;
    }

    printf("Data read (reversed): %s\n", buf);

    close(fd);
    return 0;
}

