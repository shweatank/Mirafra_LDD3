#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct calc_data {
    int a;
    int b;
    char op;
    int result;
}d;

int main()
{
    int fd = open("/dev/mywait", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    read(fd, &d, sizeof(d));   // Read result from kernel

    printf("Result of %d %c %d = %d\n", d.a, d.op, d.b, d.result);

    close(fd);
    return 0;
}

