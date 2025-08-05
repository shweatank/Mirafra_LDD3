#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define PORT 8080
#define TIMEOUT_SEC 10

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[1024] = "Interrupt triggered: Sending message to server";

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form (localhost used here)
    if (inet_pton(AF_INET, "192.168.0.175", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    printf("Press Enter within 10 seconds to send message:\n");

    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);  // Watch stdin (fd 0)

    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

    if (activity > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
        getchar();  // Consume Enter key
        send(sock, message, strlen(message), 0);
        printf("Message sent to server\n");
    } else {
        printf("No interrupt triggered (timeout reached)\n");
    }

    close(sock);
    return 0;
}


