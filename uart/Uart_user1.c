#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/my_uart", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char buf[128];
    int n = read(fd, buf, sizeof(buf));
    if (n <= 0) {
        perror("read");
        close(fd);
        return 1;
    }

    buf[n] = '\0';
    printf("Received: %s\n", buf);

    // Split by comma
    char *token = strtok(buf, ",");
    int a = atoi(token);

    token = strtok(NULL, ",");
    int b = atoi(token);

    token = strtok(NULL, ",");
    char op[10];
    strncpy(op, token, sizeof(op));

    int result = 0;
    if (strcmp(op, "add") == 0)
        result = a + b;
    else if (strcmp(op, "sub") == 0)
        result = a - b;
    else if (strcmp(op, "mul") == 0)
        result = a * b;
    else if (strcmp(op, "div") == 0 && b != 0)
        result = a / b;
    else {
        snprintf(buf, sizeof(buf), "Invalid\n");
        write(fd, buf, strlen(buf));
        close(fd);
        return 1;
    }

    snprintf(buf, sizeof(buf), "Result: %d\n", result);
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}

