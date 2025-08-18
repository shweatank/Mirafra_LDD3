// receiver_pi.c
// Receives 1024-byte OLED buffer over TCP and writes directly to SSD1306 via I2C
// Compiles on Raspberry Pi: gcc receiver_pi.c -o receiver_pi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define OLED_I2C_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 64
#define OLED_BUFFER_SIZE (WIDTH * HEIGHT / 8)
#define PORT 5000

int i2c_fd;

// Send command to SSD1306
void oled_cmd(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    if (write(i2c_fd, buffer, 2) != 2) {
        perror("OLED CMD write failed");
    }
}

// Initialize SSD1306 OLED
void oled_init() {
    oled_cmd(0xAE); // Display off
    oled_cmd(0x20); oled_cmd(0x00); // Horizontal addressing mode
    oled_cmd(0xB0); // Page 0 start
    oled_cmd(0xC8); // COM scan direction remap
    oled_cmd(0x00); // Low col start
    oled_cmd(0x10); // High col start
    oled_cmd(0x40); // Start line
    oled_cmd(0x81); oled_cmd(0x7F); // Contrast
    oled_cmd(0xA1); // Segment remap
    oled_cmd(0xA6); // Normal display
    oled_cmd(0xA8); oled_cmd(0x3F); // Multiplex ratio
    oled_cmd(0xA4); // Display follows RAM
    oled_cmd(0xD3); oled_cmd(0x00); // No display offset
    oled_cmd(0xD5); oled_cmd(0x80); // Clock divide ratio
    oled_cmd(0xD9); oled_cmd(0xF1); // Pre-charge period
    oled_cmd(0xDA); oled_cmd(0x12); // COM pins
    oled_cmd(0xDB); oled_cmd(0x40); // VCOM detect
    oled_cmd(0x8D); oled_cmd(0x14); // Enable charge pump
    oled_cmd(0xAF); // Display on
}

// Draw full buffer to OLED
void oled_draw(const uint8_t *buf) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_cmd(0xB0 + page); // Set page start address
        oled_cmd(0x00);        // Low column start
        oled_cmd(0x10);        // High column start
        uint8_t data_block[1 + WIDTH];
        data_block[0] = 0x40; // Data mode
        memcpy(&data_block[1], buf + (page * WIDTH), WIDTH);
        if (write(i2c_fd, data_block, sizeof(data_block)) != sizeof(data_block)) {
            perror("OLED data write failed");
        }
    }
}

int main() {
    // Open I2C
    if ((i2c_fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        perror("I2C open failed");
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, OLED_I2C_ADDR) < 0) {
        perror("I2C address set failed");
        return 1;
    }

    oled_init();

    // TCP server setup
    int sock_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    uint8_t buffer[OLED_BUFFER_SIZE];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(sock_fd, 1) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("Receiver waiting for connection on port %d...\n", PORT);

    client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Accept failed");
        return 1;
    }

    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    // Main loop: receive and display
    while (1) {
        ssize_t received = recv(client_fd, buffer, OLED_BUFFER_SIZE, MSG_WAITALL);
        if (received <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        if (received == OLED_BUFFER_SIZE) {
            oled_draw(buffer);
        }
    }

    close(client_fd);
    close(sock_fd);
    close(i2c_fd);
    return 0;
}

