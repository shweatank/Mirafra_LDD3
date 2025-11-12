// tcp_client.c - Simple TCP client to receive video and audio files

#include <stdio.h>      // printf, perror
#include <stdlib.h>     // malloc, free, exit
#include <string.h>     // memset
#include <unistd.h>     // close, write
#include <arpa/inet.h>  // inet_pton, htons
#include <fcntl.h>      // open, O_CREAT, O_WRONLY
#include <errno.h>      // errno
#include <sys/stat.h>   // file permissions

#define SERVER_IP "192.168.0.126" // Server IP address
#define PORT 12345                 // TCP port
#define VIDEO_TYPE 1               // Type identifier for video file
#define AUDIO_TYPE 2               // Type identifier for audio file

// Helper function to ensure we read exactly 'len' bytes from the socket
void recv_all(int sock, void *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = recv(sock, (char*)buf + total, len - total, 0); // Receive chunk
        if (n <= 0) { perror("recv"); exit(1); }                   // Error or disconnect
        total += n;                                                // Update total received
    }
}

int main() {
    // Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    // Initialize server address struct
    serv_addr.sin_family = AF_INET;                   // IPv4
    serv_addr.sin_port = htons(PORT);                // Port (network byte order)
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr); // Convert IP string to binary

    // Connect to the server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server\n");

    // Loop to receive files from server
    while (1) {
        uint8_t type;    // 1-byte type header (VIDEO_TYPE / AUDIO_TYPE)
        uint32_t size;   // 4-byte file size header

        // Receive 1-byte file type
        if (recv(sock, &type, 1, 0) <= 0) break;  

        // Receive 4-byte file size
        recv_all(sock, &size, 4);
        size = ntohl(size);   // Convert from network byte order to host

        // Allocate buffer for the entire file
        char *data = malloc(size);
        recv_all(sock, data, size);  // Receive full file

        // Save the file to disk based on type
        if (type == VIDEO_TYPE) {
            int fd = open("recv_output.h265", O_CREAT | O_WRONLY, 0666);
            write(fd, data, size);
            close(fd);
            printf("Received video -> recv_output.h265\n");
        } else if (type == AUDIO_TYPE) {
            int fd = open("recv_audio.flac", O_CREAT | O_WRONLY, 0666);
            write(fd, data, size);
            close(fd);
            printf("Received audio -> recv_audio.flac\n");
        }

        free(data);  // Free memory after writing file
    }

    close(sock);    // Close TCP socket
    return 0;
}

