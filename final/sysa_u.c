#include <stdio.h>              // For printf(), perror(), fflush()
#include <stdlib.h>             // For exit codes
#include <fcntl.h>              // For open()
#include <unistd.h>             // For read(), close()
#include <string.h>             // For strlen()
#include <sys/socket.h>         // For socket(), connect(), send()
#include <arpa/inet.h>          // For sockaddr_in, inet_addr()
#include <errno.h>              // For error reporting

int main() {
    printf("[INFO] Opening device: /dev/kbd_irq_dev\n");

    // Open character device created by the kernel module
    int fd = open("/dev/kbd_irq_dev", O_RDONLY);
    if (fd < 0) {
        perror("[ERROR] Failed to open /dev/kbd_irq_dev");
        return 1;
    }
    printf("[INFO] Device opened successfully\n");

    printf("[INFO] Creating TCP socket\n");

    // Create a TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET: IPv4 address family
    // SOCK_STREAM: TCP connection type
    // 0: Use default protocol (TCP for SOCK_STREAM)
    if (sock < 0) {
        perror("[ERROR] Socket creation failed");
        close(fd);
        return 1;
    }
    printf("[INFO] Socket created successfully\n");

    // Set up the address of System B (server)
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;               // Use IPv4
    serv_addr.sin_port = htons(6666);             // Port number (host to network short)
    serv_addr.sin_addr.s_addr = inet_addr("192.168.0.74"); // IP of System B

    printf("[INFO] Connecting to System B at 192.168.0.74:6666\n");

    // Connect to System B
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[ERROR] Socket connection failed");
        close(sock);
        close(fd);
        return 1;
    }
    printf("[INFO] Connected to System B successfully\n");

    // Notify user that we're waiting for 's' key
    printf("[INFO] Waiting for 's' key press from keyboard...\n");
    fflush(stdout);  // Flush output buffer to print message immediately

    char dummy;
    // Block here until 's' key is pressed and kernel read() is unblocked
    ssize_t ret = read(fd, &dummy, 1);
    if (ret < 0) {
        perror("[ERROR] Read failed");
        close(sock);
        close(fd);
        return 1;
    }

    printf("[INFO] Key 's' detected. Proceeding to send 'start' to System B\n");

    // Send "start" message to System B
    printf("[INFO] Sending message: \"start\"\n");
    fflush(stdout);  // Ensure message is displayed before sending data

    if (send(sock, "start", strlen("start"), 0) < 0) {
        perror("[ERROR] Send failed");
    } else {
        printf("[INFO] Message sent successfully to System B\n");
    }

    // Cleanup resources
    printf("[INFO] Closing socket and device file\n");
    close(sock);
    close(fd);

    printf("[INFO] System A process completed\n");
    return 0;
}

