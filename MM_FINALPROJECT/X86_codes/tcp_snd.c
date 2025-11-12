// tcp_server.c - Simple TCP server to send video and audio files

#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit, malloc, free
#include <string.h>     // memset
#include <unistd.h>     // close, read
#include <arpa/inet.h>  // htons, htonl, sockaddr_in
#include <sys/stat.h>   // fstat
#include <fcntl.h>      // open, O_RDONLY

#define PORT 12345
#define VIDEO_TYPE 1    // Type identifier for video file
#define AUDIO_TYPE 2    // Type identifier for audio file

// Function to send a file over TCP socket
void send_file(int sock, const char *filename, uint8_t type) {
    int fd = open(filename, O_RDONLY);   // Open file for reading
    if (fd < 0) { perror("open"); return; }

    struct stat st;
    fstat(fd, &st);                      // Get file size
    uint32_t size = htonl(st.st_size);   // Convert size to network byte order

    // Send type (1 byte) first
    send(sock, &type, 1, 0);
    // Send 4-byte file size next
    send(sock, &size, 4, 0);

    // Buffer to read and send file in chunks
    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        send(sock, buf, n, 0);           // Send each chunk
    }

    close(fd);
    printf("Sent file: %s\n", filename);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Allow reuse of address to avoid "Address already in use" error
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Initialize address structure
    address.sin_family = AF_INET;       // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    address.sin_port = htons(PORT);     // Set port (network byte order)

    bind(server_fd, (struct sockaddr *)&address, sizeof(address)); // Bind socket
    listen(server_fd, 3);               // Listen with max 3 pending connections

    printf("Server waiting...\n");
    // Accept one client connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    // Send H.265 video file first
    send_file(new_socket, "output.h265", VIDEO_TYPE);

    // Send audio file (either WAV or FLAC)
    // send_file(new_socket, "recorded.wav", AUDIO_TYPE);
    send_file(new_socket, "output.flac", AUDIO_TYPE);

    // Close connections
    close(new_socket);
    close(server_fd);
    return 0;
}

