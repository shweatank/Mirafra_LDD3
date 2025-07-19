// x86_uart_comm.c
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int uart_fd;

void* read_thread(void* arg) {
    char buf[1];         // read one byte at a time
    char message[100];   // buffer to hold full line
    int idx = 0;

    while (1) {
        int n = read(uart_fd, buf, 1);
        if (n > 0) {
            if (buf[0] == '\n' || idx >= sizeof(message) - 1) {
                message[idx] = '\0';
                printf("\n[RPI]: %s", message);
                fflush(stdout);
                idx = 0; // reset for next message
            } else {
                message[idx++] = buf[0];
            }
        }
    }
    return NULL;
}

int main() {
    uart_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
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
        printf("\n[x86] Enter message: ");
        fgets(input, sizeof(input), stdin);
        write(uart_fd, input, strlen(input));
    }

    close(uart_fd);
    return 0;
}

