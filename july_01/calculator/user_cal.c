#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/simple"

struct calc_data {
    int op1;
    int op2;
    int choice;  // 1=add, 2=sub, 3=mul, 4=div
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

    printf("Choose operation (1=Add, 2=Subtract, 3=Multiply, 4=Divide): ");
    scanf("%d", &calc.choice);

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

