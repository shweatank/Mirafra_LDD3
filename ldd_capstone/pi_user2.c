#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PLAYER_IP "192.168.0.100"  // replace with x86 Player IP
#define PLAYER_PORT 9090
#define OLED_DEV "/dev/oled_display"  // assume this is your OLED char device

int send_file(int sockfd, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("File open failed");
        return -1;
    }

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (write(sockfd, buf, n) < 0) {
            perror("Socket write error");
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    printf("[User2] Audio file sent\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s audio.wav\n", argv[0]);
        return 1;
    }

    const char *audio_file = argv[1];

    // Connect to x86 Player
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PLAYER_PORT)
    };
    inet_pton(AF_INET, PLAYER_IP, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Socket connect failed");
        close(sockfd);
        return 1;
    }

    printf("[User2] Connected to x86 Player. Sending audio...\n");
    if (send_file(sockfd, audio_file) < 0) {
        close(sockfd);
        return 1;
    }

    // Wait for "complete"
    char msg[32] = {0};
    read(sockfd, msg, sizeof(msg));
    printf("[User2] Received from Player: %s\n", msg);

    if (strcmp(msg, "complete") == 0) {
        // Display "played" on OLED
        int oled_fd = open(OLED_DEV, O_WRONLY);
        if (oled_fd >= 0) {
            write(oled_fd, "played", 6);
            close(oled_fd);
            printf("[User2] Displayed 'played' on OLED\n");
        } else {
            perror("OLED device open failed");
        }
    }

    close(sockfd);
    return 0;
}

