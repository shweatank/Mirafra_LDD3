#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8888
#define AUDIO_FILE "received_audio.wav"
#define BUFFER_SIZE 1024

void play_audio(const char *file) {
    // Replace with actual playback command for your system
    char command[256];
    snprintf(command, sizeof(command), "aplay %s", file);
    system(command);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // Listen
    if (listen(server_fd, 1) == -1) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("Waiting for connection on port %d...\n", PORT);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    printf("Connected to Raspberry Pi. Receiving audio file...\n");

    // Open file for writing
    fp = fopen(AUDIO_FILE, "wb");
    if (!fp) {
        perror("File open failed");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    // Receive audio data
    ssize_t bytes_read;
    while ((bytes_read = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, fp);
    }

    fclose(fp);
    printf("Audio file received.\n");

    // Play audio
    play_audio(AUDIO_FILE);

    // Send "complete" back to Pi
    write(client_fd, "complete", strlen("complete"));

    close(client_fd);
    close(server_fd);

    return 0;
}

