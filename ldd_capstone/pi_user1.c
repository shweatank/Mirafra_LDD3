// user_program_1.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define UART_DEV "/dev/my_uart"
#define TRIGGER_DEV "/dev/audio_trigger"
#define AUDIO_FILE "/tmp/recorded.wav"
#define PROG2_PATH "/home/pi/user_program_2"

int main() {
    int trig_fd = open(TRIGGER_DEV, O_RDONLY);
    if (trig_fd < 0) {
        perror("Failed to open trigger device");
        return 1;
    }

    printf("Waiting for interrupt...\n");
    char dummy;
    read(trig_fd, &dummy, 1); // blocks until interrupt
    close(trig_fd);
    printf("Interrupt received!\n");

    int uart_fd = open(UART_DEV, O_RDWR);
    if (uart_fd < 0) {
        perror("Failed to open UART device");
        return 1;
    }

    // Send start command
    write(uart_fd, "start", 5);
    printf("Sent 'start' to x86 recorder\n");

    // Receive .wav file
    FILE *fp = fopen(AUDIO_FILE, "wb");
    if (!fp) {
        perror("File open failed");
        close(uart_fd);
        return 1;
    }

    char buf[1024];
    ssize_t r;
    while ((r = read(uart_fd, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, r, fp);
        if (r < sizeof(buf)) break;  // assuming end of file
    }

    fclose(fp);
    close(uart_fd);
    printf("Received audio file\n");

    // Launch user program 2
    char *args[] = {PROG2_PATH, AUDIO_FILE, NULL};
    execve(PROG2_PATH, args, NULL);

    perror("execve failed");
    return 1;
}

