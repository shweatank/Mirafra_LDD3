#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int operand1;
    int operand2;
    char operator;
    int result;
} Calc;

int main() {
    int fd = open("/dev/calc_dev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    Calc c;
    c.operand1 = 20;
    c.operand2 = 5;
    c.operator = '/';

    write(fd, &c, sizeof(Calc));
    read(fd, &c, sizeof(Calc));

    printf("Result of %d %c %d = %d\n", c.operand1, c.operator, c.operand2, c.result);

    close(fd);
    return 0;
}

