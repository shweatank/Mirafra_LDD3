// x86_B_user.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6000
#define UARTCHAR_DEV "/dev/uartchar"
#define UARTALERT_DEV "/dev/uartalert"

// External alert control functions
extern void play_alert();
extern void stop_alert();

void *alert_listener(void *arg) {
    int alert_fd;
    char ch;

    alert_fd = open(UARTALERT_DEV, O_RDONLY);
    if (alert_fd < 0) {
        perror("[x86_B_user] Failed to open /dev/uartalert");
        return NULL;
    }

    while (1) {
        if (read(alert_fd, &ch, 1) == 1) {
            if (ch == 'x') {
                printf("[x86_B_user] ALERT received (x) → Playing alert.wav\n");
                play_alert();
            } else if (ch == 'z') {
                printf("[x86_B_user] STOP received (z) → Stopping alert\n");
                stop_alert();
            }
        }
    }

    close(alert_fd);
    return NULL;
}

int main() {
    int server_fd, new_socket, uart_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t alert_thread;
    char buffer[64];
    int current_speed, max_speed;

    uart_fd = open(UARTCHAR_DEV, O_WRONLY | O_SYNC);
    if (uart_fd < 0) {
        perror("[x86_B_user] Failed to open /dev/uartchar");
        return 1;
    }

    pthread_create(&alert_thread, NULL, alert_listener, NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[x86_B_user] Socket creation failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("[x86_B_user] Bind failed");
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("[x86_B_user] Listen failed");
        return 1;
    }

    printf("[x86_B_user] Listening on port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("[x86_B_user] Accept failed");
            continue;
        }

        printf("[x86_B_user] Connection accepted from x86_A\n");

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(new_socket, buffer, sizeof(buffer));
            if (valread <= 0) {
                printf("[x86_B_user] Client disconnected.\n");
                break;
            }

            if (sscanf(buffer, "%d %d", &current_speed, &max_speed) == 2) {
                printf("[x86_B_user] Received: Current = %d, Max = %d\n", current_speed, max_speed);

                char choice;
                printf("Do you want to modify the speed? (y/n): ");
                scanf(" %c", &choice);

                if (choice == 'y') {
                    printf("Enter new current speed: ");
                    scanf("%d", &current_speed);
                    printf("Enter new max speed: ");
                    scanf("%d", &max_speed);
                }

                // UART string with newline and flush
                char uart_data[32];
                snprintf(uart_data, sizeof(uart_data), "%d %d", current_speed, max_speed);
                write(uart_fd, uart_data, strlen(uart_data));
                write(uart_fd, "\n", 1);  // Important: send newline
                fsync(uart_fd);           // Ensure it's flushed to device

                printf("[x86_B_user] Sent to Raspberry Pi: %s\n", uart_data);
            } else {
                printf("[x86_B_user] Invalid format received: %s\n", buffer);
            }
        }

        close(new_socket);
    }

    close(server_fd);
    close(uart_fd);
    pthread_cancel(alert_thread);
    pthread_join(alert_thread, NULL);

    return 0;
}

