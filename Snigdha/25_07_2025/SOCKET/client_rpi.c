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

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Server IP and Port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 🧠 Replace this with the actual IP of your x86 PC
    if (inet_pton(AF_INET, "192.168.0.175", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address or Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    // Send message
    char *msg = "Hello from Raspberry Pi!";
    send(sock, msg, strlen(msg), 0);
    printf("Client: Message sent\n");

    // Receive response
    read(sock, buffer, sizeof(buffer));
    printf("Client: Server says -> %s\n", buffer);

    close(sock);
    return 0;
}

