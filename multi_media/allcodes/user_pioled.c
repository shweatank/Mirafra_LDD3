#include <stdio.h>              // For printf(), perror()
#include <stdint.h>             // For fixed-size integer types like uint8_t
#include <stdlib.h>             // For exit()
#include <string.h>             // For strlen(), etc.
#include <fcntl.h>              // For open()
#include <linux/i2c-dev.h>      // For I2C ioctl definitions
#include <sys/ioctl.h>          // For ioctl()
#include <unistd.h>             // For write(), close()

// Macros defining OLED device and screen configuration
#define I2C_DEVICE "/dev/i2c-1"       // Path to I2C device file
#define SSD1306_ADDR 0x3C              // I2C address of the OLED display
#define WIDTH 128                      // Width of the display in pixels
#define HEIGHT 32                      // Height of the display in pixels (used to set multiplex ratio)

int i2c_fd;  // Global file descriptor for I2C communication

// 5x8 pixel font for characters A-Z (each letter has 5 bytes + 1 spacing byte)
const uint8_t font5x8[][5] = {
    // Font bitmaps for uppercase A-Z
    {0x7E, 0x11, 0x11, 0x7E, 0x00}, // A
    {0x7F, 0x49, 0x49, 0x36, 0x00}, // B
    {0x3E, 0x41, 0x41, 0x22, 0x00}, // C
    {0x7F, 0x41, 0x41, 0x3E, 0x00}, // D
    {0x7F, 0x49, 0x49, 0x41, 0x00}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x00}, // F
    {0x3E, 0x41, 0x51, 0x72, 0x00}, // G
    {0x7F, 0x08, 0x08, 0x7F, 0x00}, // H
    {0x41, 0x7F, 0x41, 0x00, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x00}, // J
    {0x7F, 0x08, 0x14, 0x63, 0x00}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x00}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x06, 0x18, 0x7F, 0x00}, // N
    {0x3E, 0x41, 0x41, 0x3E, 0x00}, // O
    {0x7F, 0x09, 0x09, 0x06, 0x00}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x66, 0x00}, // R
    {0x26, 0x49, 0x49, 0x32, 0x00}, // S
    {0x01, 0x7F, 0x01, 0x00, 0x00}, // T
    {0x3F, 0x40, 0x40, 0x3F, 0x00}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x30, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
};

// Send a single command byte to SSD1306
void ssd1306_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};  // 0x00 indicates command
    write(i2c_fd, buffer, 2);
}

// Initialize SSD1306 OLED with required setup commands
void ssd1306_init() {
    ssd1306_command(0xAE);             // Display OFF
    ssd1306_command(0xD5);             // Set display clock divide ratio
    ssd1306_command(0x80);             // Default clock
    ssd1306_command(0xA8);             // Set multiplex ratio
    ssd1306_command(HEIGHT - 1);       // 32 - 1
    ssd1306_command(0xD3);             // Display offset
    ssd1306_command(0x00);             // No offset
    ssd1306_command(0x40);             // Start line address = 0
    ssd1306_command(0x8D);             // Charge pump
    ssd1306_command(0x14);             // Enable charge pump
    ssd1306_command(0x20);             // Memory addressing mode
    ssd1306_command(0x00);             // Horizontal addressing mode
    ssd1306_command(0xA1);             // Segment remap
    ssd1306_command(0xC8);             // COM scan direction
    ssd1306_command(0xDA);             // COM pins config
    ssd1306_command(0x02);             // Alternative COM config
    ssd1306_command(0x81);             // Contrast control
    ssd1306_command(0xCF);             // Set contrast
    ssd1306_command(0xD9);             // Pre-charge period
    ssd1306_command(0xF1);
    ssd1306_command(0xDB);             // VCOMH deselect level
    ssd1306_command(0x40);
    ssd1306_command(0xA4);             // Entire display ON from RAM
    ssd1306_command(0xA6);             // Normal display (not inverted)
    ssd1306_command(0xAF);             // Display ON
}

// Clear entire screen by writing 0x00 to each pixel in each page
void ssd1306_clear() {
    for (uint8_t page = 0; page < 4; page++) {
        ssd1306_command(0xB0 + page);  // Set page address
        ssd1306_command(0x00);         // Lower column start
        ssd1306_command(0x10);         // Higher column start
        for (uint8_t col = 0; col < WIDTH; col++) {
            uint8_t buffer[2] = {0x40, 0x00};  // Data byte = 0x00
            write(i2c_fd, buffer, 2);
        }
    }
}

// Set cursor to specific column and page
void ssd1306_set_cursor(uint8_t col, uint8_t page) {
    ssd1306_command(0xB0 + page);                 // Set page
    ssd1306_command(0x00 + (col & 0x0F));         // Lower nibble of column
    ssd1306_command(0x10 + ((col >> 4) & 0x0F));  // Upper nibble of column
}

// Draw a single capital letter using font5x8[]
void ssd1306_draw_char(char c) {
    if (c < 'A' || c > 'Z') return;               // Skip unsupported characters
    const uint8_t* bitmap = font5x8[c - 'A'];     // Get character bitmap
    for (int i = 0; i < 5; i++) {
        uint8_t buf[2] = {0x40, bitmap[i]};       // 0x40 = data prefix
        write(i2c_fd, buf, 2);
    }
    uint8_t space[2] = {0x40, 0x00};              // Add spacing between characters
    write(i2c_fd, space, 2);
}

// Write a string of capital letters to screen
void ssd1306_draw_string(const char* str) {
    ssd1306_set_cursor(0, 0);  // Start at top-left corner
    while (*str) {
        ssd1306_draw_char(*str);  // Draw each character
        str++;
    }
}

int main() {
    i2c_fd = open(I2C_DEVICE, O_RDWR);              // Open I2C device file
    if (i2c_fd < 0) {
        perror("Failed to open I2C device");        // Error opening
        exit(1);
    }

    if (ioctl(i2c_fd, I2C_SLAVE, SSD1306_ADDR) < 0) {
        perror("Failed to set I2C address");       // Error configuring device address
        close(i2c_fd);
        exit(1);
    }

    ssd1306_init();               // Initialize display
    ssd1306_clear();              // Clear display

    printf("Displaying HELLO on OLED...\n");
    ssd1306_draw_string("PLAYED SUCCESSFULLY");  // Show message on screen

    close(i2c_fd);                // Close I2C file
    return 0;                     // Exit program
}

