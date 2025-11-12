// Compile: gcc -o ili9225 ili9225.c -lwiringPi
// Run: sudo ./ili9225

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <wiringPi.h>
#include <string.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED  10000000   // 10 MHz

// Control pins (wiringPi numbering)
#define PIN_RST  5   // GPIO24 → Reset
#define PIN_RS   6   // GPIO25 → D/C

int spi_fd;

// ========== SPI Functions ==========
void spiWrite(uint8_t data) {
    write(spi_fd, &data, 1);
}

void lcd_cmd(uint8_t cmd) {
    digitalWrite(PIN_RS, LOW);  // Command mode
    spiWrite(cmd);
}

void lcd_data(uint8_t data) {
    digitalWrite(PIN_RS, HIGH); // Data mode
    spiWrite(data);
}

void lcd_data16(uint16_t data) {
    lcd_data(data >> 8);
    lcd_data(data & 0xFF);
}

// ========== LCD Control ==========
void lcd_reset() {
    digitalWrite(PIN_RST, LOW);
    usleep(50000);
    digitalWrite(PIN_RST, HIGH);
    usleep(50000);
}

// ILI9225 Initialization
void lcd_init() {
    lcd_reset();

    lcd_cmd(0x01); lcd_data16(0x011C);
    lcd_cmd(0x02); lcd_data16(0x0100);
    lcd_cmd(0x03); lcd_data16(0x1030);
    lcd_cmd(0x08); lcd_data16(0x0808);
    lcd_cmd(0x0C); lcd_data16(0x0000);
    lcd_cmd(0x0F); lcd_data16(0x0C01);
    lcd_cmd(0x20); lcd_data16(0x0000);
    lcd_cmd(0x21); lcd_data16(0x0000);

    lcd_cmd(0x10); lcd_data16(0x0A00);
    lcd_cmd(0x11); lcd_data16(0x1038);
    lcd_cmd(0x12); lcd_data16(0x1121);
    lcd_cmd(0x13); lcd_data16(0x0063);

    // Gamma
    lcd_cmd(0x30); lcd_data16(0x0000);
    lcd_cmd(0x31); lcd_data16(0x00DB);
    lcd_cmd(0x32); lcd_data16(0x0000);
    lcd_cmd(0x33); lcd_data16(0x0000);
    lcd_cmd(0x34); lcd_data16(0x00DB);
    lcd_cmd(0x35); lcd_data16(0x0000);
    lcd_cmd(0x36); lcd_data16(0x00AF);
    lcd_cmd(0x37); lcd_data16(0x0000);
    lcd_cmd(0x38); lcd_data16(0x00DB);
    lcd_cmd(0x39); lcd_data16(0x0000);

    // Display ON
    lcd_cmd(0x07); lcd_data16(0x1017);
}

// Convert RGB888 → RGB565
uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) |
           ((g & 0xFC) << 3) |
           (b >> 3);
}

// ========== Display BMP Image ==========
void lcd_draw_bmp(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, f);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    short bitsPerPixel = *(short*)&header[28];

    if (width != 176 || height != 220 || bitsPerPixel != 24) {
        printf("Image must be 176x220, 24-bit BMP!\n");
        fclose(f);
        return;
    }

    int row_padded = (width * 3 + 3) & (~3);
    unsigned char *row = malloc(row_padded);

    lcd_cmd(0x20); lcd_data16(0);
    lcd_cmd(0x21); lcd_data16(0);
    lcd_cmd(0x22);

    digitalWrite(PIN_RS, HIGH);

    // BMP is stored bottom-up
    for (int y = height - 1; y >= 0; y--) {
        fread(row, sizeof(unsigned char), row_padded, f);
        for (int x = 0; x < width; x++) {
            unsigned char b = row[x*3 + 0];
            unsigned char g = row[x*3 + 1];
            unsigned char r = row[x*3 + 2];
            lcd_data16(rgb565(r,g,b));
        }
    }

    free(row);
    fclose(f);
}

// ========== Main ==========
int main() {
    wiringPiSetup();
    pinMode(PIN_RST, OUTPUT);
    pinMode(PIN_RS, OUTPUT);

    // Open SPI
    spi_fd = open(SPI_DEVICE, O_WRONLY);
    if (spi_fd < 0) { perror("SPI open"); exit(1); }
    uint32_t speed = SPI_SPEED;
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    lcd_init();

    // Display only the BMP image
    lcd_draw_bmp("output.bmp");

    close(spi_fd);
    return 0;
}

