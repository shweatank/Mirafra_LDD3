#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/add_driver"

int main() {
    int fd;
    char input[100], output[100];
    ssize_t n;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    int a, b, c;
    printf("Enter 3 integers: ");
    scanf("%d %d %d", &a, &b, &c);

    snprintf(input, sizeof(input), "%d %d %d", a, b, c);
    write(fd, input, strlen(input));

    n = read(fd, output, sizeof(output) - 1);
    output[n] = '\0';

    printf("Sum from kernel: %s\n", output);

    close(fd);
    return 0;
}

