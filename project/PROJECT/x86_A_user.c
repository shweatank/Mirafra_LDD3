#include <stdio.h>              // For printf(), scanf(), perror()
#include <fcntl.h>              // For open() system call and O_RDONLY flag
#include <unistd.h>             // For read(), close()
#include <sys/socket.h>         // For socket(), connect(), send()
#include <arpa/inet.h>          // For sockaddr_in, inet_pton(), htons()
#include <string.h>             // For memset()

#define SERVER_IP   "192.168.0.150"  // IP address of the x86B machine (receiver system)
#define SERVER_PORT 9000             // TCP port on which the x86B is listening

int main() {
    // Open the character device file created by the kernel module (kbdchar)
    int fd = open("/dev/kbdchar", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Waiting for key press...\n");

    while (1) {
        unsigned char sc;
        int ret = read(fd, &sc, 1);
        if (ret < 0) {
            perror("read");
            break;
        }

        // If 's' key is pressed (0x1F is scancode for 's')
        if (sc == 0x1F) {
            printf("Matched key 's'. Sending speed data...\n");

            int speed, max_speed;

            // Loop until valid input is entered
            while (1) {
                printf("Enter speed (>= 0): ");
                if (scanf("%d", &speed) != 1 || speed < 0) {
                    printf("Invalid speed. Please enter a non-negative number.\n");
                    while (getchar() != '\n'); // Flush invalid input
                    continue;
                }

                printf("Enter max speed (>= 0): ");
                if (scanf("%d", &max_speed) != 1 || max_speed < 0) {
                    printf("Invalid max speed. Please enter a non-negative number.\n");
                    while (getchar() != '\n');
                    continue;
                }

                if (speed > max_speed) {
                    printf("Speed cannot be greater than max speed. Try again.\n");
                    continue;
                }

                break; // Valid input
            }

            // Create TCP socket
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                perror("socket");
                continue;
            }

            struct sockaddr_in server_addr = {
                .sin_family = AF_INET,
                .sin_port = htons(SERVER_PORT),
            };
            inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

            if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("connect");
                close(sockfd);
                continue;
            }

            int data[2] = {speed, max_speed};
            if (send(sockfd, data, sizeof(data), 0) < 0) {
                perror("send");
            } else {
                printf("Data sent: speed=%d, max_speed=%d\n", speed, max_speed);
            }

            close(sockfd);
        }
    }

    close(fd);
    return 0;
}

