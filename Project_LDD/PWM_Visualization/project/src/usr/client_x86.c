#include <stdio.h>              // Standard I/O functions
#include <sys/socket.h>        // For socket(), connect(), send()
#include <netinet/in.h>        // For sockaddr_in
#include <unistd.h>            // For close()
#include <string.h>            // For memset, strcpy, etc.
#include <fcntl.h>             // For open()

int main() {
    char msg[256] = {0};       // Buffer to store the message read from the character device

    // Create a TCP socket
    int ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ser_sock == -1) {
        printf("Failed to create socket\n");
        return -1;
    }

    // Open the custom character device file
    int fd = open("/dev/characters_pressed", O_RDWR);
    if (fd < 0) {
        perror("open failed"); // Print error if open fails
        return 1;
    }

    // Read data from the character device into msg buffer
    int n = read(fd, msg, sizeof(msg));
    msg[n] = '\0';  // Null-terminate the string read

    // Create and set up the server address struct
    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;                 // IPv4
    ser_addr.sin_port = htons(8080);               // Port number in network byte order
    ser_addr.sin_addr.s_addr = inet_addr("192.168.0.105"); // Server IP address

    // Connect to the server
    if (connect(ser_sock, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1) {
        printf("Connection failed\n");
        return -1;
    }

    // Send the message read from the device to the server
    send(ser_sock, msg, sizeof(msg), 0);
    printf("Message sent to server: %s\n", msg);

    // Close the socket after sending the message
    close(ser_sock);

    return 0;
}

