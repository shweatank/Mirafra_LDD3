// x86_uart_recorder.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define UART_DEV "/dev/ttyUSB0"
#define TEMP_WAV "recorded.wav"

void setup_uart(int fd) {
    struct termios tty;
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);    // enable receiver
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8-bit
    tty.c_cflag &= ~PARENB;             // no parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // no flow control

    tcsetattr(fd, TCSANOW, &tty);
}

int main() {
    int fd = open(UART_DEV, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open UART");
        return 1;
    }
    setup_uart(fd);

    char start_buf[16] = {0};
    printf("Waiting for 'start' from Pi...\n");
    read(fd, start_buf, 5);
    printf("Received: %s\n", start_buf);

    // Start recording
    printf("Recording...\n");
    system("arecord -d 3 -f cd -t wav -r 44100 -c 1 " TEMP_WAV);

    FILE *fp = fopen(TEMP_WAV, "rb");
    if (!fp) {
        perror("Cannot open .wav file");
        close(fd);
        return 1;
    }

    char buf[1024];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), fp)) > 0) {
        write(fd, buf, r);
    }

    fclose(fp);
    close(fd);
    printf("Sent audio to Pi\n");
    return 0;
}

