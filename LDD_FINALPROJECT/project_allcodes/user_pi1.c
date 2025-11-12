#include <stdio.h>      // Standard I/O functions (e.g., fopen, printf)
#include <fcntl.h>      // For open() flags like O_RDONLY
#include <unistd.h>     // For read(), write(), close(), sleep(), fork(), exec
#include <stdlib.h>     // For exit(), malloc(), free(), system()
#include <string.h>     // For string operations like strlen(), strcmp()
#include <sys/ioctl.h>  // For ioctl() system call
#include <sys/wait.h>   // For wait() and related macros
#include <sys/socket.h> // For socket-related functions
#include <netinet/in.h> // For sockaddr_in structure and constants
#include <arpa/inet.h>  // For inet_ntoa() and related functions

#define UART_DEV "/dev/my_uart"         // UART device file to communicate with x86 board
#define TRIGGER_DEV "/dev/audio_trigger" // Device file that waits for an external interrupt
#define AUDIO_FILE "recorded.wav"         // Name of the audio file to save received data
#define PROG2_PATH "./pi_user2"           // Optional path for second user-space program
#define PORT 8080                         // Port number for socket server
#define BUFFER_SIZE 1024                  // Buffer size for receiving audio data

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;         // Structure to hold socket address
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Open the trigger device and wait for interrupt
    int trig_fd = open(TRIGGER_DEV, O_RDONLY);   // Open the audio_trigger device
    if (trig_fd < 0) {
        perror("Failed to open trigger device"); // Error if open fails
        return 1;
    }

    printf("Waiting for interrupt...\n");
    char dummy;
    read(trig_fd, &dummy, 1);   // This blocks until the interrupt is received
    close(trig_fd);             // Close trigger device after interrupt
    printf("Interrupt received!\n");

    // Open UART device
    int uart_fd = open(UART_DEV, O_RDWR);
    if (uart_fd < 0)
    {
        perror("Failed to open UART device");
        return 1;
    }

    // Send start command to x86 via UART
    write(uart_fd, "start", 5);
    printf("Sent 'start' to x86 recorder\n");

    // Create socket for communication
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(1);
    }

    // Bind the socket to a local address and port
    address.sin_family = AF_INET;              // IPv4
    address.sin_addr.s_addr = INADDR_ANY;      // Accept any incoming interface
    address.sin_port = htons(PORT);            // Set port with correct byte order
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen for incoming connections (1 client)
    listen(server_fd, 1);
    printf("Waiting for connection on port %d...\n", PORT);

    //Accept a new client connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Client connected.\n");

    // Open a file to save the received audio data
    fp = fopen("received.wav", "wb");
    if (!fp) {
        perror("File open error");
        close(new_socket);
        exit(1);
    }

    //Receive data from socket and write to file
    int bytes_received;
    while ((bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);  // Write received data to file
    }

    printf("File received successfully.\n");

    //Cleanup
    fclose(fp);           // Close file
    close(new_socket);    // Close client socket
    close(server_fd);     // Close server socket

    printf("Received audio file\n");

    return 0;
}

