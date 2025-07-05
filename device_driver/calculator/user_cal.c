#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct calc_data {
    int operand1;
    int operand2;
    char operator;
};

int main() {
    int fd;
    struct calc_data data;

    fd = open("/dev/simple_cal_dev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter operand1: ");
    scanf("%d", &data.operand1);
    printf("Enter operand2: ");
    scanf("%d", &data.operand2);
    printf("Enter operator (+ - * /): ");
    scanf(" %c", &data.operator);

    // Write struct to kernel
    if (write(fd, &data, sizeof(data)) < 0) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    // Read result from kernel
    int result;
	read(fd, &result, sizeof(result));
	printf("Result = %d\n", result);
      close(fd);
    return 0;
}

