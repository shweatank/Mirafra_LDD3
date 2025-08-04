#include <stdio.h>              // Standard I/O functions
#include <fcntl.h>              // File control definitions (open, etc.)
#include <unistd.h>             // For close(), read(), write()
#include <string.h>             // String handling
#include <termios.h>            // For UART configuration
#include <sys/socket.h>         // For socket APIs
#include <netinet/in.h>         // For sockaddr_in structure

// Function to set up the UART device with appropriate settings
int setup_uart(const char *device) {
    int fd = open(device, O_WRONLY | O_NOCTTY);  // Open UART device for writing, no controlling terminal
    if (fd < 0) {
        perror("UART open failed");              // Print error if opening fails
        return -1;
    }

    struct termios options;                     // Structure for UART config options
    tcgetattr(fd, &options);                    // Get current attributes

    cfsetispeed(&options, B9600);               // Set input baud rate
    cfsetospeed(&options, B9600);               // Set output baud rate

    options.c_cflag |= (CLOCAL | CREAD);        // Enable receiver, set local mode
    options.c_cflag &= ~CSIZE;                  // Clear current character size mask
    options.c_cflag |= CS8;                     // Set 8 data bits
    options.c_cflag &= ~PARENB;                 // Disable parity
    options.c_cflag &= ~CSTOPB;                 // 1 stop bit
    options.c_cflag &= ~CRTSCTS;                // Disable hardware flow control

    tcsetattr(fd, TCSANOW, &options);           // Apply settings immediately
    return fd;                                  // Return file descriptor
}

int main() {
    char msg[200];                              // Buffer to hold received message

    // Setup UART and check if successful
    int uart_fd = setup_uart("/dev/ttyUSB0");   // Specify the UART device
    if (uart_fd < 0) return 1;

    // Create a TCP socket
    int ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ser_sock == -1) {
        perror("Socket creation failed");       // Error message if socket creation fails
        return -1;
    }

    // Server address setup
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;              // IPv4
    ser_addr.sin_port = htons(8080);            // Port 8080 in network byte order
    ser_addr.sin_addr.s_addr = INADDR_ANY;      // Accept connections from any IP

    // Bind the socket to the address and port
    if (bind(ser_sock, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1) {
        perror("Bind failed");                  // Error if bind fails
        return -1;
    }

    // Start listening for incoming connections
    if (listen(ser_sock, 5) == -1) {
        perror("Listen failed");                // Error if listen fails
        return -1;
    }

    printf("Server listening on port 8080...\n"); // Print listening message

    // Accept client connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(ser_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock == -1) {
        perror("Accept failed");                // Error if accept fails
        return -1;
    }

    // Receive message from client
    int n = recv(client_sock, msg, sizeof(msg) - 1, 0);
    if (n > 0) {
        msg[n] = '\0';                          // Null-terminate the received message
        printf("Received: %s\n", msg);          // Print the message
        write(uart_fd, msg, strlen(msg));       // Send the message to UART device
    }

    // Close all sockets and file descriptors
    close(client_sock);                         // Close client socket
    close(ser_sock);                            // Close server socket
    close(uart_fd);                             // Close UART file descriptor

    return 0;                                   // Exit the program
}

