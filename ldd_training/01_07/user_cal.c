#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct calc {
    int operand1;
    int operand2;
    char operator;
    int result;
};

int main() {
    int fd;
    struct calc calc;
    char ch;

    fd = open("/dev/calc", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Enter operator (+ - * /): ");
    scanf(" %c", &ch);

    printf("Enter two integers: ");
    scanf("%d %d", &calc.operand1, &calc.operand2);

    calc.operator = ch;

    write(fd, &calc, sizeof(calc));
    read(fd, &calc, sizeof(calc));

    switch (ch) {
        case '+':
        case '-':
        case '*':
        case '/':
            printf("Result: %d\n", calc.result);
            break;
        default:
            printf("Invalid operator!\n");
    }

    close(fd);
    return 0;
}

