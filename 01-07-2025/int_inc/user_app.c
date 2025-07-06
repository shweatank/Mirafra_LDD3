#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd, val;

    fd = open("/dev/int_increment", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    val = 4;
    write(fd, &val, sizeof(int));  // send 10
    read(fd, &val, sizeof(int));   // receive incremented value

    printf("Value from kernel: %d\n", val);  // Should print 11

    close(fd);
    return 0;
}

