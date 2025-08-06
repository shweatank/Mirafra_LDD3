// x86_A_user.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.0.104"  // IP of x86_B
#define SERVER_PORT 7000
#define DEVICE "/dev/kbdnotify"

int wait_for_keypress() {
    int fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Device open failed");
        return -1;
    }

    char ch;
    printf("[x86_A_user] Waiting for key press (s to send, q to quit)...\n");

    while (1) {
        int n = read(fd, &ch, 1);
        if (n > 0) {
            close(fd);
            return ch;
        }
        usleep(100000);
    }
}

int main() {
    while (1) {
        char key = wait_for_keypress();

        if (key == 'q') {
            printf("[x86_A_user] Quit key pressed. Exiting...\n");
            break;
        }

        if (key == 's') {
            printf("[x86_A_user] 's' pressed. Connecting to x86_B...\n");

            int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_fd < 0) {
                perror("Socket creation failed");
                continue;
            }

            struct sockaddr_in server_addr = {
                .sin_family = AF_INET,
                .sin_port = htons(SERVER_PORT),
                .sin_addr.s_addr = inet_addr(SERVER_IP)
            };

            if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("Connection failed");
                close(sock_fd);
                continue;
            }

            int current_speed = 0, max_speed = 0;
            char input[100];

            printf("Enter current speed: ");
            if (fgets(input, sizeof(input), stdin))
                sscanf(input, "%d", &current_speed);

            printf("Enter max speed: ");
            if (fgets(input, sizeof(input), stdin))
                sscanf(input, "%d", &max_speed);

            char msg[64];
            snprintf(msg, sizeof(msg), "%d %d\n", current_speed, max_speed);
            write(sock_fd, msg, strlen(msg));

            printf("[x86_A_user] Sent: Current = %d, Max = %d\n", current_speed, max_speed);
            close(sock_fd);
        }
    }

    return 0;
}

