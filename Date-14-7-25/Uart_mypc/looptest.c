#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = CS8 | CLOCAL | CREAD;
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tty);

    printf("Starting UART loopback test on /dev/ttyUSB0...\n");

    char ch = 'A';
    while (1) {
        // Write 1 byte
        write(fd, &ch, 1);
        printf("Sent: '%c'\n", ch);

        // Read 1 byte
        char rx = 0;
        ssize_t ret = read(fd, &rx, 1);

        if (ret > 0)
            printf("Received: '%c'\n\n", rx);
        else
            printf("Timeout or no data received\n");

        ch++;
        if (ch > 'Z') ch = 'A';

        sleep(1);  // Wait a second before next byte
    }

    close(fd);
    return 0;
}

