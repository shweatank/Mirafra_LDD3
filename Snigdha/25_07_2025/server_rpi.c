// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    socklen_t addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to IP/port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accept from any IP
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    listen(server_fd, 3);
    printf("Server: Listening on port %d...\n", PORT);

    // Accept connection
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Read from client
    read(client_fd, buffer, sizeof(buffer));
    printf("Server: Received -> %s\n", buffer);

    // Respond
    char *reply = "Hello Raspberry Pi!";
    send(client_fd, reply, strlen(reply), 0);
    printf("Server: Reply sent.\n");

    close(client_fd);
    close(server_fd);
    return 0;
}

