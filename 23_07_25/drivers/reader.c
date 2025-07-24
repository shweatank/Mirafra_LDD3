// uart_reader.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/dual_uart", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char ch;
    read(fd, &ch, 1);
    printf("Received & processed: %c\n", ch);
    close(fd);
    return 0;
}

