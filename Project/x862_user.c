// Standard input/output functions like printf(), perror(), snprintf()
#include <stdio.h>

// For system() call used to play audio
#include <stdlib.h>

// For string handling functions like memset(), strcmp(), strcspn()
#include <string.h>

// For close() and sleep functions
#include <unistd.h>

// For file control options, though not strictly used here (can be omitted)
#include <fcntl.h>

// For IP address conversion (inet_addr)
#include <arpa/inet.h>

// For socket programming (socket(), bind(), listen(), accept())
#include <sys/socket.h>

// Define the TCP port number on which this x86 system will listen
#define PORT 8080

// Function to play the audio using system command
void play_audio(const char *audio_file) {
    char cmd[64];
    // Form the command to play audio (e.g., aplay alert_audio1.wav)
    snprintf(cmd, sizeof(cmd), "aplay %s", audio_file);
    system(cmd);  // Execute the command using the shell
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    char buffer[64];

    // 1. Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Setup server address
    server.sin_family = AF_INET;               // IPv4
    server.sin_addr.s_addr = inet_addr("0.0.0.0");  // Accept from any IP
    server.sin_port = htons(PORT);             // Convert to network byte order

    // 3. Bind the socket to the port and IP
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // 4. Start listening for incoming connections (backlog = 3)
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("[x86_2] Listening on port %d...\n", PORT);

    // 5. Run an infinite loop to accept and handle connections
    while (1) {
        // Accept a new incoming connection
        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;  // Continue to next iteration
        }

        // Clear buffer and read incoming message
        memset(buffer, 0, sizeof(buffer));
        int len = read(client_fd, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            // Sanitize input: remove trailing newline or carriage return
            buffer[strcspn(buffer, "\r\n")] = 0;

            printf("[x86_2] Received: %s\n", buffer);

            // Play corresponding audio based on message content
            if (strcmp(buffer, "alert_audio1.wav") == 0) {
                play_audio("alert_audio1.wav");
            } else if (strcmp(buffer, "alert_audio2.wav") == 0) {
                play_audio("alert_audio2.wav");
            } else {
                printf("Unknown alert: %s\n", buffer);
            }
        }

        // Close the client socket after handling one alert
        close(client_fd);
    }

    // Close the main server socket (not reachable in infinite loop)
    close(server_fd);
    return 0;
}

