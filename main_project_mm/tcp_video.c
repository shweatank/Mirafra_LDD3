// tcp_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 12345
#define VIDEO_TYPE 1
#define AUDIO_TYPE 2

void send_file(int sock, const char *filename, uint8_t type) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    struct stat st;
    fstat(fd, &st);
    uint32_t size = htonl(st.st_size);

    send(sock, &type, 1, 0);
    send(sock, &size, 4, 0);

    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        send(sock, buf, n, 0);
    }
    close(fd);
    printf("Sent file: %s\n", filename);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server waiting...\n");
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    // Send H.265 video file
    send_file(new_socket, "output.h265", VIDEO_TYPE);

    // Send audio file
   // send_file(new_socket, "recorded.wav", AUDIO_TYPE);
    send_file(new_socket, "output.flac", AUDIO_TYPE);

    close(new_socket);
    close(server_fd);
    return 0;
}

