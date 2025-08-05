#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/spi_sd_loop", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char write_buf[] = "HELLOSD";
    char read_buf[512];

    write(fd, write_buf, sizeof(write_buf));
    read(fd, read_buf, sizeof(write_buf));

    printf("Received from SD (SPI): %s\n", read_buf);

    close(fd);
    return 0;
}

