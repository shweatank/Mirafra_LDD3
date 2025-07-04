#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
enum calc_op {
    OP_ADD = 0,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MAX
};
struct calc_data {
    int a;
    int b;
    enum calc_op op;
    int result;
};

int main() {
    int fd;
    struct calc_data cd;
    fd = open("/dev/calc_enum_1", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/calculator_enum");
        return 1;
    }
    cd.a = 20;
    cd.b = 5;
    cd.op = OP_DIV;
    if (write(fd, &cd, sizeof(cd)) < 0) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    lseek(fd, 0, SEEK_SET); 
    if (read(fd, &cd, sizeof(cd)) < 0) {
        perror("Read failed");
        close(fd);
        return 1;
    }
    const char *ops[] = {"+", "-", "*", "/"};
    if (cd.op >= 0 && cd.op < 4)
        printf("Result of %d %s %d = %d\n", cd.a, ops[cd.op], cd.b, cd.result);
    else
        printf("Unknown operation\n");
    close(fd);
    return 0;
}

