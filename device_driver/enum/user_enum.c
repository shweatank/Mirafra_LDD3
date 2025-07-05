#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

enum Command {
    CMD_HELLO = 1,
    CMD_BYE = 2
};

int main() {
    int fd, cmd, result;

    fd = open("/dev/simple_enum_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    cmd = CMD_HELLO;
    write(fd, &cmd, sizeof(int));

    read(fd, &result, sizeof(int));
    printf("Result from kernel: %d\n", result);

    close(fd);
    return 0;
}

