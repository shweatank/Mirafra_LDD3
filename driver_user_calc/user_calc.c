#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE "/dev/calculator"

struct calc_data {
    int a;
    int b;
    char op;
    int result;
};

int main()
{
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    struct calc_data calc;
    calc.a = 10;
    calc.b = 20;
    calc.op = '+';
    calc.result = 0;
    ssize_t written = write(fd, &calc, sizeof(calc));
    if (written != sizeof(calc)) {
        perror("Failed to write full struct");
        close(fd);
        return 1;
    }
    printf("Sent: %d %c %d\n", calc.a, calc.op, calc.b);
    memset(&calc, 0, sizeof(calc));
    ssize_t read_bytes = read(fd, &calc, sizeof(calc));
    if (read_bytes != sizeof(calc)) {
        perror("Failed to read full struct");
        close(fd);
        return 1;
    }

    printf("Result: %d %c %d = %d\n", calc.a, calc.op, calc.b, calc.result);

    close(fd);
    return 0;
}

