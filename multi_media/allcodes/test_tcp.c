// x86_audio_sender.c

#include <stdio.h>          // Standard I/O functions like printf, fopen, etc.
#include <stdlib.h>         // Standard library for exit(), malloc(), etc.
#include <unistd.h>         // For sleep(), close(), etc.
#include <string.h>         // For memory operations like memset, memcpy, etc.
#include <arpa/inet.h>      // For sockaddr_in, inet_pton(), htons(), etc.

#define PI_IP "192.168.0.153"      // IP address of the Raspberry Pi (receiver/server)
#define PORT 8080                  // Port number that the Pi is listening on
#define BUFFER_SIZE 1024           // Size of the buffer used for sending file chunks

int main() {
    sleep(5);  // Delay the execution for 5 seconds to allow receiver setup (optional)

    int sock;                           // Socket descriptor
    struct sockaddr_in server_addr;     // Structure to hold server address info
    char buffer[BUFFER_SIZE];           // Buffer for reading file chunks
    FILE *fp;                           // File pointer for audio file
    size_t bytes_read;                  // Number of bytes read from file

    // Step 1: Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP
    if (sock < 0) {
        perror("socket");              // Print error if socket creation fails
        exit(EXIT_FAILURE);            // Exit the program with failure code
    }

    // Step 2: Set up server address (Raspberry Pi)
    server_addr.sin_family = AF_INET;                      // IPv4
    server_addr.sin_port = htons(PORT);                    // Convert port to network byte order
    inet_pton(AF_INET, PI_IP, &server_addr.sin_addr);      // Convert IP address from text to binary

    // Step 3: Connect to the Raspberry Pi's socket
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");            // Print error if connection fails
        close(sock);                  // Close the socket
        exit(EXIT_FAILURE);           // Exit the program
    }

    // Step 4: Open the audio file in binary read mode
    fp = fopen("recorded.wav", "rb"); // Must use "rb" to read binary data
    if (fp == NULL) {
        perror("fopen");              // Print error if file can't be opened
        close(sock);                  // Close the socket
        exit(EXIT_FAILURE);           // Exit the program
    }

    // Step 5: Read file in chunks and send over socket
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        if (send(sock, buffer, bytes_read, 0) != bytes_read) {
            perror("send");          // Print error if sending fails
            fclose(fp);              // Close the file
            close(sock);             // Close the socket
            exit(EXIT_FAILURE);      // Exit the program
        }
    }

    // Step 6: Clean up
    fclose(fp);                       // Close the audio file
    close(sock);                      // Close the socket

    printf("Audio file sent successfully\n"); // Confirm success to user
    return 0;                        // Exit with success
}

