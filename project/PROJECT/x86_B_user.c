#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>     // For sockaddr_in structure
#include <sys/socket.h>     // For socket(), bind(), listen(), accept()
#include <fcntl.h>          // For open() function
#include <termios.h>        // For UART configuration
#include <errno.h>          // For error reporting

// Define TCP server port and UART device
#define PORT 9000
#define UART_DEVICE "/dev/ttyUSB0"

// Function to set up UART communication
int setup_uart(const char *device) {
    // Open the UART device in read-write mode, no controlling terminal, non-blocking
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("UART open failed");  // If open fails, print error
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);                 // Get current UART settings
    cfsetispeed(&options, B115200);          // Set input baud rate to 115200
    cfsetospeed(&options, B115200);          // Set output baud rate to 115200

    options.c_cflag &= ~PARENB;              // Disable parity bit
    options.c_cflag &= ~CSTOPB;              // Use one stop bit
    options.c_cflag &= ~CSIZE;               // Clear current data size setting
    options.c_cflag |= CS8;                  // Use 8 data bits
    options.c_cflag |= (CREAD | CLOCAL);     // Enable receiver, ignore modem control lines

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input (non-canonical mode)
    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
    options.c_oflag &= ~OPOST;                          // Raw output (no post-processing)

    tcsetattr(fd, TCSANOW, &options);        // Apply the settings immediately

    return fd;   // Return UART file descriptor
}

int main() {
    int server_fd, client_fd;               // File descriptors for TCP server and accepted client
    struct sockaddr_in address;             // Struct for server address
    socklen_t addrlen = sizeof(address);    // Length of address struct

    // Create TCP socket (IPv4, Stream socket, default protocol)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return 1;
    }

    // Fill in the server address structure
    address.sin_family = AF_INET;                // IPv4
    address.sin_addr.s_addr = INADDR_ANY;        // Accept connections from any IP
    address.sin_port = htons(PORT);              // Convert port to network byte order

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    // Start listening for incoming connections (max 3 pending connections)
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return 1;
    }

    printf("x86B Server listening on port %d...\n", PORT);

    // Setup UART port
    int uart_fd = setup_uart(UART_DEVICE);
    if (uart_fd < 0) return 1;  // Exit if UART setup fails

    while (1) {
        printf("\nWaiting for connection from x86A...\n");

        // Accept incoming connection from x86A
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0) {
            perror("accept");
            continue;  // Retry accepting
        }

        int data[2];  // Array to hold received speed and max_speed
        int bytes = recv(client_fd, data, sizeof(data), 0);  // Receive data from x86A

        if (bytes == sizeof(data)) {
            printf("Received from x86A: speed = %d, max_speed = %d\n", data[0], data[1]);

            // Format data into a string to send over UART
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "Speed:%d Max:%d\n", data[0], data[1]);

            // Send the formatted string to Raspberry Pi over UART
            write(uart_fd, buffer, strlen(buffer));
            printf("Sent to Raspberry Pi: %s", buffer);
        } else {
            printf("Incomplete data received %d bytes\n", bytes);
        }

        close(client_fd);  // Close client connection
    }

    close(uart_fd);  // Close UART device before exiting
    return 0;        // Exit main
}

