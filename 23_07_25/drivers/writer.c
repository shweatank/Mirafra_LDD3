// uart_writer.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/dual_uart", O_WRONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char ch = 'a';
    write(fd, &ch, 1);
    printf("Sent: %c\n", ch);
    close(fd);
    return 0;
}

