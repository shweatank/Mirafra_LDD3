#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE "/dev/simple"

enum operation {
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_DIV = 4
};

struct calc_data {
    int op1;
    int op2;
    enum operation choice;
    int result;
};

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    struct calc_data calc;

    printf("Enter first number: ");
    scanf("%d", &calc.op1);

    printf("Enter second number: ");
    scanf("%d", &calc.op2);

    printf("Choose operation:\n");
    printf("1 = Add, 2 = Subtract, 3 = Multiply, 4 = Divide\n");
    int op;
    scanf("%d", &op);

    if (op < OP_ADD || op > OP_DIV) {
        fprintf(stderr, "Invalid operation\n");
        close(fd);
        return 1;
    }

    calc.choice = (enum operation)op;

    if (write(fd, &calc, sizeof(calc)) != sizeof(calc)) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    if (read(fd, &calc, sizeof(calc)) != sizeof(calc)) {
        perror("Read failed");
        close(fd);
        return 1;
    }

    printf("Result: %d\n", calc.result);
    close(fd);
    return 0;
}

