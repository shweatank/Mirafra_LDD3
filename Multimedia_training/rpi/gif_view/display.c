#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <gif_lib.h>

#define I2C_DEV "/dev/i2c-1"
#define SSD1306_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 32

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA    0x40

int i2c_fd;

// Send a single command byte to SSD1306
void ssd1306_command(unsigned char cmd) {
    unsigned char buffer[2] = {SSD1306_COMMAND, cmd};
    if (write(i2c_fd, buffer, 2) != 2) {
        perror("I2C Write Command failed");
    }
}

// Send multiple data bytes
void ssd1306_data(const unsigned char *data, size_t len) {
    unsigned char buf[1 + WIDTH];
    buf[0] = SSD1306_DATA;
    size_t sent = 0;
    while (sent < len) {
        size_t chunk = (len - sent > WIDTH) ? WIDTH : len - sent;
        buf[0] = SSD1306_DATA;
        memcpy(buf + 1, data + sent, chunk);
        if (write(i2c_fd, buf, chunk + 1) != (ssize_t)(chunk + 1)) {
            perror("I2C Write Data failed");
            break;
        }
        sent += chunk;
    }
}

// Initialize SSD1306 for 128x32
void ssd1306_init() {
    ssd1306_command(0xAE); // Display off
    ssd1306_command(0xA8); ssd1306_command(HEIGHT - 1);
    ssd1306_command(0xD3); ssd1306_command(0x00);
    ssd1306_command(0x40);
    ssd1306_command(0xA1);
    ssd1306_command(0xC8);
    ssd1306_command(0xDA); ssd1306_command(0x02);
    ssd1306_command(0x81); ssd1306_command(0x8F);
    ssd1306_command(0xA4);
    ssd1306_command(0xA6);
    ssd1306_command(0xD5); ssd1306_command(0x80);
    ssd1306_command(0x8D); ssd1306_command(0x14);
    ssd1306_command(0x20); ssd1306_command(0x00);
    ssd1306_command(0xAF); // Display on
}

// Clear screen
void ssd1306_clear() {
    unsigned char zero[WIDTH] = {0};
    for (int page = 0; page < (HEIGHT / 8); page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        ssd1306_data(zero, WIDTH);
    }
}

// Draw monochrome bitmap buffer (size WIDTH * HEIGHT/8)
void ssd1306_draw_bitmap(unsigned char *buf) {
    for (int page = 0; page < (HEIGHT / 8); page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        ssd1306_data(buf + page * WIDTH, WIDTH);
    }
}

// Convert GIF frame to monochrome buffer
void gif_frame_to_mono(GifFileType *gif, SavedImage *frame, unsigned char *mono_buf) {
    memset(mono_buf, 0, (WIDTH * HEIGHT) / 8);
    ColorMapObject *cmap = frame->ImageDesc.ColorMap ? frame->ImageDesc.ColorMap : gif->SColorMap;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int idx = y * WIDTH + x;
            int colorIndex = frame->RasterBits[idx];
            GifColorType color = cmap->Colors[colorIndex];
            int gray = (color.Red + color.Green + color.Blue) / 3;
            if (gray < 128) {
                mono_buf[(y / 8) * WIDTH + x] |= (1 << (y % 8)); // Set pixel
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s animation.gif\n", argv[0]);
        return 1;
    }

    // Open I²C device
    if ((i2c_fd = open(I2C_DEV, O_RDWR)) < 0) {
        perror("Failed to open I2C device");
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, SSD1306_ADDR) < 0) {
        perror("Failed to set I2C address");
        return 1;
    }

    // Init display
    ssd1306_init();
    ssd1306_clear();

    // Open GIF
    int err;
    GifFileType *gif = DGifOpenFileName(argv[1], &err);
    if (!gif) {
        fprintf(stderr, "Error opening GIF: %d\n", err);
        return 1;
    }
    if (DGifSlurp(gif) != GIF_OK) {
        fprintf(stderr, "Failed to read GIF\n");
        DGifCloseFile(gif, &err);
        return 1;
    }

    unsigned char mono_buf[(WIDTH * HEIGHT) / 8];

    // Loop animation forever
    while (1) {
        for (int f = 0; f < gif->ImageCount; f++) {
            gif_frame_to_mono(gif, &gif->SavedImages[f], mono_buf);
            ssd1306_draw_bitmap(mono_buf);
            usleep(100000); // 100ms per frame
        }
    }

    DGifCloseFile(gif, &err);
    close(i2c_fd);
    return 0;
}

