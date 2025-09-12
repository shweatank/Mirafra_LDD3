// tcp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define SERVER_IP "192.168.0.126"
#define PORT 12345
#define VIDEO_TYPE 1
#define AUDIO_TYPE 2

void recv_all(int sock, void *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = recv(sock, (char*)buf + total, len - total, 0);
        if (n <= 0) { perror("recv"); exit(1); }
        total += n;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server\n");

    while (1) {
        uint8_t type;
        uint32_t size;

        if (recv(sock, &type, 1, 0) <= 0) break;
        recv_all(sock, &size, 4);
        size = ntohl(size);

        char *data = malloc(size);
        recv_all(sock, data, size);

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

        free(data);
    }

    close(sock);
    return 0;
}

