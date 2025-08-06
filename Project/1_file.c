#include <stdio.h>                  // Standard I/O functions
#include <stdint.h>                // Fixed-width integer types (e.g., uint8_t)
#include <stdlib.h>                // Standard library functions (e.g., exit)
#include <string.h>                // String manipulation functions (e.g., snprintf)
#include <fcntl.h>                 // File control options (open, etc.)
#include <linux/i2c-dev.h>         // I2C device definitions
#include <sys/ioctl.h>             // For ioctl system call
#include <unistd.h>                // For close(), write(), etc.

#define I2C_DEVICE "/dev/i2c-1"    // I2C bus device file for Raspberry Pi
#define SSD1306_ADDR 0x3C          // I2C address of the OLED SSD1306
#define WIDTH 128                  // OLED width in pixels
#define HEIGHT 32                  // OLED height in pixels

int i2c_fd;                        // Global file descriptor for the I2C device

// 5x8 pixel font definitions for characters A–Z, 0–9, and a few symbols
const uint8_t font5x8[][5] = {
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
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
    {0x22, 0x41, 0x49, 0x49, 0x36}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x2F, 0x49, 0x49, 0x49, 0x31}, // 5
    {0x3E, 0x49, 0x49, 0x49, 0x32}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x26, 0x49, 0x49, 0x49, 0x3E}, // 9
    {0x00, 0x18, 0x18, 0x00, 0x00}, // =
    {0x00, 0x00, 0x00, 0x00, 0x00}, // (space)
};

// Send a single command byte to the OLED
void ssd1306_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};    // 0x00 = command control byte
    write(i2c_fd, buffer, 2);           // Write command to OLED
}

// Initialize the SSD1306 OLED with standard setup commands
void ssd1306_init() {
    ssd1306_command(0xAE); // Display OFF
    ssd1306_command(0xD5); ssd1306_command(0x80); // Set display clock
    ssd1306_command(0xA8); ssd1306_command(HEIGHT - 1); // Set multiplex
    ssd1306_command(0xD3); ssd1306_command(0x00); // Display offset
    ssd1306_command(0x40); // Set start line to 0
    ssd1306_command(0x8D); ssd1306_command(0x14); // Charge pump ON
    ssd1306_command(0x20); ssd1306_command(0x00); // Horizontal addressing
    ssd1306_command(0xA0); // Segment remap
    ssd1306_command(0xC0); // COM scan direction
    ssd1306_command(0xDA); ssd1306_command(0x02); // COM pins config
    ssd1306_command(0x81); ssd1306_command(0xCF); // Contrast
    ssd1306_command(0xD9); ssd1306_command(0xF1); // Precharge
    ssd1306_command(0xDB); ssd1306_command(0x40); // VCOMH deselect
    ssd1306_command(0xA4); // Entire display ON resume
    ssd1306_command(0xA6); // Normal display
    ssd1306_command(0xAF); // Display ON
}

// Clear the entire OLED display
void ssd1306_clear() {
    for (uint8_t page = 0; page < 4; page++) {
        ssd1306_command(0xB0 + page);       // Set page start address
        ssd1306_command(0x00);              // Lower column
        ssd1306_command(0x10);              // Higher column
        for (uint8_t col = 0; col < WIDTH; col++) {
            uint8_t buffer[2] = {0x40, 0x00}; // Data control byte + 0x00
            write(i2c_fd, buffer, 2);        // Write blank byte to clear
        }
    }
}

// Set cursor to specific column and page
void ssd1306_set_cursor(uint8_t col, uint8_t page) {
    ssd1306_command(0xB0 + page);                   // Set page address
    ssd1306_command(0x00 + (col & 0x0F));           // Set lower nibble of column
    ssd1306_command(0x10 + ((col >> 4) & 0x0F));    // Set upper nibble
}

// Draw a single character using bitmap
void ssd1306_draw_char(char c) {
    const uint8_t* bitmap = NULL;

    if (c >= 'A' && c <= 'Z') {
        bitmap = font5x8[c - 'A'];            // Get bitmap for A–Z
    } else if (c >= '0' && c <= '9') {
        bitmap = font5x8[26 + (c - '0')];     // Get bitmap for 0–9
    } else if (c == '=') {
        bitmap = font5x8[36];                 // Get bitmap for '='
    } else if (c == ' ') {
        bitmap = font5x8[37];                 // Get bitmap for space
    } else {
        return;                               // Unsupported character
    }

    for (int i = 0; i < 5; i++) {
        uint8_t buf[2] = {0x40, bitmap[i]};   // Data control byte + bitmap column
        write(i2c_fd, buf, 2);                // Write character pixel
    }
    uint8_t space[2] = {0x40, 0x00};          // Add one column space between chars
    write(i2c_fd, space, 2);
}

// Draw a string of characters starting from beginning
void ssd1306_draw_string(const char* str) {
    ssd1306_set_cursor(0, 0);    // Set to top-left
    while (*str) {
        ssd1306_draw_char(*str++); // Draw one character at a time
    }
}

// Main program entry
int main(int argc, char *argv[]) {
    char display_msg[64]; // Buffer to hold final message

    if (argc == 3) {
        printf("temp:%s thr:%s\n", argv[1], argv[2]); // Debug print
        snprintf(display_msg, sizeof(display_msg), "TEMP=%s THRESH:%s", argv[1], argv[2]); // Format message
    } else if (argc == 2) {
        snprintf(display_msg, sizeof(display_msg), "%s", argv[1]); // to display LOCKED
    }

    i2c_fd = open(I2C_DEVICE, O_RDWR);   // Open the I2C bus
    if (i2c_fd < 0) {
        perror("Failed to open I2C device"); // Print error if failed
        exit(1);
    }

    if (ioctl(i2c_fd, I2C_SLAVE, SSD1306_ADDR) < 0) { // Set the slave address
        perror("Failed to set I2C address"); // Print error if failed
        close(i2c_fd);
        exit(1);
    }

    ssd1306_init();          // Initialize OLED
    ssd1306_clear();         // Clear display
    ssd1306_draw_string(display_msg); // Draw formatted message
    close(i2c_fd);           // Close I2C file descriptor

    return 0;                // Exit program
}
