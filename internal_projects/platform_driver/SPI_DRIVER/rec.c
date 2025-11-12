// receiver_rpi.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#define LCD_WIDTH   176
#define LCD_HEIGHT  220
#define FRAME_SIZE  (LCD_WIDTH * LCD_HEIGHT * 2)  // RGB565

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <port> <lcd_device>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    const char *lcd_dev = argv[2];

    // Open LCD driver device
    int lcd_fd = open(lcd_dev, O_WRONLY);
    if (lcd_fd < 0) {
        perror("open lcd device");
        return 1;
    }

    // Setup TCP socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("bind");
        return 1;
    }
    listen(listen_fd, 1);
    printf("Waiting for sender on port %d...\n", port);

    int conn = accept(listen_fd, NULL, NULL);
    if (conn < 0) { perror("accept"); return 1; }
    printf("Sender connected\n");

    uint8_t *framebuf = malloc(FRAME_SIZE);
    if (!framebuf) {
        perror("malloc");
        close(conn);
        return 1;
    }

    while (1) {
        // Read 4-byte length header
        uint32_t netlen;
        int r = recv(conn, &netlen, 4, MSG_WAITALL);
        if (r <= 0) break;
        int payload = ntohl(netlen);

        if (payload != FRAME_SIZE) {
            printf("Unexpected frame size: %d\n", payload);
            char *dummy = malloc(payload);
            recv(conn, dummy, payload, MSG_WAITALL);
            free(dummy);
            continue;
        }

        // Read frame
        r = recv(conn, framebuf, FRAME_SIZE, MSG_WAITALL);
        if (r <= 0) break;

        printf("Received frame: %d bytes\n", r);

        // Send frame to LCD driver
        int written = write(lcd_fd, framebuf, FRAME_SIZE);
        if (written < 0) {
            perror("write to lcd");
            break;
        }
    }

    free(framebuf);
    close(conn);
    close(lcd_fd);
    return 0;
}
