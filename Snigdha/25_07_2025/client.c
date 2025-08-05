// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    // 2. Server address setup
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Replace with server PC's IP
    if (inet_pton(AF_INET, "192.168.0.66", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address / Address not supported\n");
        return -1;
    }

    // 3. Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    // 4. Send data
    char *msg = "Hello from client!";
    send(sock, msg, strlen(msg), 0);

    // 5. Read response
    read(sock, buffer, sizeof(buffer));
    printf("Server says: %s\n", buffer);

    close(sock);
    return 0;
}

