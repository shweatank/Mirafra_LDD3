#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define I2C_DEVICE "/dev/i2c-1"
#define SSD1306_ADDR 0x3C

#define WIDTH 128
#define HEIGHT 32

int i2c_fd;

void ssd1306_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    write(i2c_fd, buffer, 2);
}

void ssd1306_init() {
    ssd1306_command(0xAE); // Display OFF
    ssd1306_command(0xD5); // Set display clock divide
    ssd1306_command(0x80); // the suggested ratio
    ssd1306_command(0xA8); // Set multiplex
    ssd1306_command(HEIGHT - 1);
    ssd1306_command(0xD3); // Display offset
    ssd1306_command(0x00);
    ssd1306_command(0x40); // Start line = 0
    ssd1306_command(0x8D); // Charge pump
    ssd1306_command(0x14);
    ssd1306_command(0x20); // Memory mode
    ssd1306_command(0x00); // Horizontal
    ssd1306_command(0xA1); // Segment remap
    ssd1306_command(0xC8); // COM scan dec
    ssd1306_command(0xDA); // COM pins
    ssd1306_command(0x02);
    ssd1306_command(0x81); // Contrast
    ssd1306_command(0xCF);
    ssd1306_command(0xD9); // Pre-charge
    ssd1306_command(0xF1);
    ssd1306_command(0xDB); // VCOM detect
    ssd1306_command(0x40);
    ssd1306_command(0xA4); // Resume display
    ssd1306_command(0xA6); // Normal display
    ssd1306_command(0xAF); // Display ON
}

void ssd1306_clear() {
    for (uint8_t page = 0; page < 4; page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        for (uint8_t col = 0; col < WIDTH; col++) {
            uint8_t buffer[2] = {0x40, 0x00};  // data
            write(i2c_fd, buffer, 2);
        }
    }
}

// Just draws one hardcoded char pattern (e.g. "A")
void ssd1306_draw_char_A() {
    // 5x8 font for 'A'
    uint8_t A[5] = {
        0x7E, // 01111110
        0x11, // 00010001
        0x11, // 00010001
        0x7E, // 01111110
        0x00  // spacer
    };

    ssd1306_command(0xB0);  // page 0
    ssd1306_command(0x00);  // lower column start
    ssd1306_command(0x10);  // higher column start

    for (int i = 0; i < 5; i++) {
        uint8_t buf[2] = {0x40, A[i]};
        write(i2c_fd, buf, 2);
    }
}

int main() {
    // Open I2C bus
    i2c_fd = open(I2C_DEVICE, O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open I2C device");
        exit(1);
    }

    if (ioctl(i2c_fd, I2C_SLAVE, SSD1306_ADDR) < 0) {
        perror("Failed to set I2C address");
        close(i2c_fd);
        exit(1);
    }

    ssd1306_init();
    ssd1306_clear();

    printf("Displaying A on OLED...\n");
    ssd1306_draw_char_A();

    close(i2c_fd);
    return 0;
}

