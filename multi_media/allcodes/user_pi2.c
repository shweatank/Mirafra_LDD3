#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PLAYER_IP "192.168.0.241"  // IP address of the x86 system which plays the audio
#define PLAYER_PORT 9090             // Port number on x86 where it listens for incoming audio
#define UART_DEV "/dev/my_uart"     // UART device through which UART communication occurs
#define PROG3_PATH "./pi_oled"       // Path to user-space program to be executed upon completion

// Function to send a file over a connected socket
int send_file(int sockfd, const char *filename) 
{
    FILE *fp = fopen(filename, "rb");  // Open the audio file in binary read mode
    if (!fp) {
        perror("File open failed");   // Print error if file couldn't be opened
        return -1;
    }

    char buf[1024];                     // Buffer to hold chunks of data from the file
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) { // Read file in chunks
        if (write(sockfd, buf, n) < 0) {               // Send each chunk through socket
            perror("Socket write error");
            fclose(fp);
            return -1;                                 // Exit if there's a socket error
        }
    }

    fclose(fp);                                        // Close the file after sending
    printf("[User2] Audio file sent\n");               // Confirmation log
    return 0;
}

int main() {
    printf("sleep\n");
    sleep(5); // Optional delay if required for synchronization (currently commented out)

    const char *audio_file = "received.wav"; // Name of audio file to send to x86

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1; // Exit if socket creation fails
    }

    //  Prepare the server address struct to connect to the x86 system
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,                 // IPv4
        .sin_port = htons(PLAYER_PORT)        // Convert port to network byte order
    };
    inet_pton(AF_INET, PLAYER_IP, &serv_addr.sin_addr); // Convert IP string to binary form

    // Connect to x86 machine
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Socket connect failed");
        close(sockfd); // Close socket on error
        return 1;
    }

    printf("[User2] Connected to x86 Player. Sending audio...\n");

    // Send the audio file over the socket
    if (send_file(sockfd, audio_file) < 0) {
        close(sockfd); // Close socket if file sending fails
        return 1;
    }

    close(sockfd); // Close the socket after file is sent

    // Open UART device to read the completion message from the 2nd x86
    int uart_fd = open(UART_DEV, O_RDONLY);
    if (uart_fd < 0) {
        perror("Failed to open UART device");
        return 1;
    }

    char msg[32] = {0};                      // Buffer to store message read from UART
    ssize_t n = read(uart_fd, msg, sizeof(msg)); // Read message from UART device
    close(uart_fd);                          // Close UART device after reading

    if (n > 0) {
        printf("[User2] Received from 2nd x86  (UART): %s\n", msg); // Print UART message
        system(PROG3_PATH); // Launch pi_oled 
    } else {
        perror("UART read failed or empty response"); // Print error if read fails
    }

}

