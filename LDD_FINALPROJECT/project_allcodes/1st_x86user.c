#include <stdio.h>      // For printf(), perror()
#include <stdlib.h>     // For system(), exit codes
#include <string.h>     // For memory functions (not used directly here)
#include <unistd.h>     // For read(), close()
#include <fcntl.h>      // For open() and file control options
#include <termios.h>    // For UART configuration via termios

#define UART_DEV "/dev/ttyUSB0"  // UART device file path (adjust as needed)

void setup_uart(int fd) {
    struct termios tty;

    // Get current UART attributes into tty structure
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr failed");  // Print error if unable to get attributes
        return;
    }

    // Set both input and output baud rates to 115200
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // Enable receiver and set local mode (ignore modem control lines)
    tty.c_cflag |= (CLOCAL | CREAD);

    // Clear the current character size mask
    tty.c_cflag &= ~CSIZE;

    // Set character size to 8 bits
    tty.c_cflag |= CS8;

    // Disable parity bit
    tty.c_cflag &= ~PARENB;

    // Use one stop bit
    tty.c_cflag &= ~CSTOPB;

    // Disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;

    // Set raw input mode: disable canonical mode, echo, and signal chars
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Disable software flow control (XON/XOFF)
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Set raw output mode
    tty.c_oflag &= ~OPOST;

    // Apply UART settings immediately
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");  // Error if settings couldn't be applied
    }
}

int main() {
    // Open UART device for read/write, no controlling terminal
    int fd = open(UART_DEV, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open UART");
        return 1;
    }

    // Configure UART using custom setup function
    setup_uart(fd);

    char start_buf[16] = {0};  // Buffer to receive incoming UART data

    printf("Waiting for 'start' from Raspberry Pi...\n");

    // Blocking read: wait until at least 1 byte is received
    while (read(fd, start_buf, 5) <= 0);

    // Print received UART message (e.g., "start")
    printf("Received UART command: %s\n", start_buf);

    close(fd);                 // Close UART file descriptor

    system("./audio");         // Runs audio recording...

    return 0;                  // Exit main function
}

