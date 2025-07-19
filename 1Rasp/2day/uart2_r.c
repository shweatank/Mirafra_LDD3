// rpi_uart_comm.c
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int uart_fd;

void* read_thread(void* arg) {
    char buf[100];
    while (1) {
        int n = read(uart_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("\n[x86]: %s", buf);
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    uart_fd = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Failed to open UART");
        return 1;
    }

    struct termios tty;
    tcgetattr(uart_fd, &tty);
    cfsetspeed(&tty, B115200);
    tty.c_cflag = CS8 | CLOCAL | CREAD;
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &tty);

    pthread_t reader;
    pthread_create(&reader, NULL, read_thread, NULL);

    char input[100];
    while (1) {
        printf("\n[Pi] Enter message: ");
        fgets(input, sizeof(input), stdin);
        write(uart_fd, input, strlen(input));
    }

    close(uart_fd);
    return 0;
}

