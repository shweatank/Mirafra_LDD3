#include <stdio.h>              // Standard I/O functions
#include <stdlib.h>             // General utilities (e.g., malloc, rand, system)
#include <fcntl.h>              // File control options (e.g., open)
#include <unistd.h>             // POSIX API (e.g., read, write, close)
#include <termios.h>            // Terminal I/O interface for configuring serial ports
#include <string.h>             // String manipulation (e.g., memset, strcmp)
#include <time.h>               // Time handling (e.g., time, srand)
#include <arpa/inet.h>          // Internet operations (e.g., sockaddr_in, htons)
#include <sys/select.h>         // select() API for I/O multiplexing

#define SERIAL_PORT "/dev/ttyUSB0"   // Serial device to communicate with Raspberry Pi
#define SERVER_PORT 6666             // TCP server port number
#define BUF_SIZE 128                 // Buffer size for receiving commands

// Function to play audio using aplay command
void play_audio(const char *filename) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "aplay %s", filename); // Build the command to play audio
    int ret = system(cmd); // Execute the command
    if (ret != 0) {
        fprintf(stderr, "Audio play failed for: %s\n", filename); // Log on failure
    }
}

// Function to configure and open the serial port
int setup_serial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY); // Open the device for read/write, no controlling terminal
    if (fd < 0) {
        perror("Failed to open serial port");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty); // Clear the termios structure

    // Get current serial port attributes
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr error");
        close(fd);
        return -1;
    }

    // Set input/output baud rate to 115200
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);    // Enable receiver and ignore modem control lines
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8 data bits
    tty.c_cflag &= ~PARENB;             // No parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // No hardware flow control

    tty.c_lflag = 0;                    // No canonical mode, echo off, etc.
    tty.c_oflag = 0;                    // No output processing
    tty.c_cc[VMIN]  = 1;                // Read at least 1 character
    tty.c_cc[VTIME] = 10;               // Timeout in tenths of a second

    tcflush(fd, TCIFLUSH);             // Flush input buffer

    // Set new attributes immediately
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr error");
        close(fd);
        return -1;
    }

    return fd; // Return serial file descriptor
}

int main() {
    int sockfd, client_fd, serial_fd;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // Initialize server address structure
    serv_addr.sin_family = AF_INET;                // IPv4
    serv_addr.sin_port = htons(SERVER_PORT);       // Port in network byte order
    serv_addr.sin_addr.s_addr = INADDR_ANY;        // Accept connections on any interface

    // Bind the socket to the port
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }

    // Listen for incoming connections
    listen(sockfd, 1);
    printf("System B: Waiting for connection from System A...\n");

    // Accept a connection
    client_fd = accept(sockfd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept failed");
        close(sockfd);
        return 1;
    }
    printf("System B: Client connected.\n");

    // Wait to receive the "start" command
    memset(buf, 0, BUF_SIZE); // Clear buffer
    ssize_t bytes = read(client_fd, buf, BUF_SIZE - 1); // Read command from socket
    if (bytes <= 0) {
        perror("read failed or connection closed");
        close(client_fd);
        close(sockfd);
        return 1;
    }
    buf[bytes] = '\0'; // Null-terminate the string
    printf("System B: Received command: '%s'\n", buf);

    // Check if it's a valid "start" command
    if (strncmp(buf, "start", 5) != 0) {
        printf("Invalid command received: %s\n", buf);
        close(client_fd);
        close(sockfd);
        return 1;
    }

    printf("System B: Received 'start' command from System A\n");

    // Play audio file "start.wav"
    play_audio("start.wav");

    // Generate random number between 1 and 4
    srand(time(NULL)); // Seed the random number generator
    int num = (rand() % 4) + 1;
    char msg[32];
    snprintf(msg, sizeof(msg), "%d\n", num); // Format number as string
    printf("System B: Generated number: %d\n", num);

    // Open the serial port to communicate with Raspberry Pi
    serial_fd = setup_serial(SERIAL_PORT);
    if (serial_fd < 0) {
        close(client_fd);
        close(sockfd);
        return 1;
    }

    // Send the number to Raspberry Pi over UART
    ssize_t written = write(serial_fd, msg, strlen(msg));
    if (written != (ssize_t)strlen(msg)) {
        perror("Failed to write full data to serial");
        close(serial_fd);
        close(client_fd);
        close(sockfd);
        return 1;
    }
    printf("System B: Sent to RPi: %s", msg);

    /*
    // The following code waits for response from Raspberry Pi
    char response = 0;
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);                 // Clear the set
    FD_SET(serial_fd, &read_fds);       // Add serial fd to the set
    timeout.tv_sec = 5;                 // Wait 5 seconds
    timeout.tv_usec = 0;

    int ret = select(serial_fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret == -1) {
        perror("select error");
    } else if (ret == 0) {
        printf("System B: Timeout: No response from Raspberry Pi\n");
    } else {
        ssize_t r = read(serial_fd, &response, 1);
        if (r > 0) {
            printf("System B: Received response from RPi(hex): '%02x'\n", response);
            if (response == '1') {
                printf("System B: RPi says: SUCCESS\n");
                play_audio("success.wav");
            } else {
                printf("System B: RPi says: FAILURE\n");
                play_audio("failure.wav");
            }
        } else {
            printf("System B: Error reading from serial\n");
        }
    }
    */

    // Clean up and close all file descriptors
    close(serial_fd);
    close(client_fd);
    close(sockfd);
    printf("System B: Exiting.\n");

    return 0;
}

