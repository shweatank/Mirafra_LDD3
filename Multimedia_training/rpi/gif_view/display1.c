#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <gif_lib.h>
#include <math.h>

#define I2C_DEV "/dev/i2c-1"
#define SSD1306_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 32

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA    0x40

int i2c_fd;

// Send command to SSD1306
void ssd1306_command(unsigned char cmd) {
    unsigned char buffer[2] = {SSD1306_COMMAND, cmd};
    write(i2c_fd, buffer, 2);
}

// Send data to SSD1306
void ssd1306_data(const unsigned char *data, size_t len) {
    unsigned char buf[1 + WIDTH];
    buf[0] = SSD1306_DATA;
    size_t sent = 0;
    while (sent < len) {
        size_t chunk = (len - sent > WIDTH) ? WIDTH : len - sent;
        buf[0] = SSD1306_DATA;
        memcpy(buf + 1, data + sent, chunk);
        write(i2c_fd, buf, chunk + 1);
        sent += chunk;
    }
}

// Initialize SSD1306
void ssd1306_init() {
    ssd1306_command(0xAE);
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
    ssd1306_command(0xAF);
}

// Clear display
void ssd1306_clear() {
    unsigned char zero[WIDTH] = {0};
    for (int page = 0; page < (HEIGHT / 8); page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        ssd1306_data(zero, WIDTH);
    }
}

// Draw bitmap
void ssd1306_draw_bitmap(unsigned char *buf) {
    for (int page = 0; page < (HEIGHT / 8); page++) {
        ssd1306_command(0xB0 + page);
        ssd1306_command(0x00);
        ssd1306_command(0x10);
        ssd1306_data(buf + page * WIDTH, WIDTH);
    }
}

// Scale RGB image to 128x32
void scale_image(unsigned char *src, int src_w, int src_h, unsigned char *dst) {
    double x_ratio = (double)src_w / WIDTH;
    double y_ratio = (double)src_h / HEIGHT;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int src_x = (int)(x * x_ratio);
            int src_y = (int)(y * y_ratio);
            int src_idx = (src_y * src_w + src_x) * 3;
            dst[(y * WIDTH + x) * 3 + 0] = src[src_idx + 0];
            dst[(y * WIDTH + x) * 3 + 1] = src[src_idx + 1];
            dst[(y * WIDTH + x) * 3 + 2] = src[src_idx + 2];
        }
    }
}

// Convert scaled RGB to monochrome buffer
void rgb_to_mono(unsigned char *rgb, unsigned char *mono) {
    memset(mono, 0, (WIDTH * HEIGHT) / 8);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int idx = (y * WIDTH + x) * 3;
            int gray = (rgb[idx] + rgb[idx+1] + rgb[idx+2]) / 3;
            if (gray < 128)
                mono[(y/8)*WIDTH + x] |= (1 << (y%8));
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s animation.gif\n", argv[0]);
        return 1;
    }

    // Open I2C
    if ((i2c_fd = open(I2C_DEV, O_RDWR)) < 0) {
        perror("I2C open");
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, SSD1306_ADDR) < 0) {
        perror("I2C address");
        return 1;
    }

    ssd1306_init();
    ssd1306_clear();

    // Open GIF
    int err;
    GifFileType *gif = DGifOpenFileName(argv[1], &err);
    if (!gif) {
        fprintf(stderr, "GIF open error %d\n", err);
        return 1;
    }
    if (DGifSlurp(gif) != GIF_OK) {
        fprintf(stderr, "GIF read error\n");
        DGifCloseFile(gif, &err);
        return 1;
    }

    unsigned char scaled_rgb[WIDTH * HEIGHT * 3];
    unsigned char mono_buf[(WIDTH * HEIGHT) / 8];

    // Loop forever
    while (1) {
        for (int f = 0; f < gif->ImageCount; f++) {
            SavedImage *frame = &gif->SavedImages[f];
            ColorMapObject *cmap = frame->ImageDesc.ColorMap ? frame->ImageDesc.ColorMap : gif->SColorMap;
            int src_w = gif->SWidth;
            int src_h = gif->SHeight;
            unsigned char *src_rgb = malloc(src_w * src_h * 3);

            // Fill RGB from GIF frame
            for (int y = 0; y < src_h; y++) {
                for (int x = 0; x < src_w; x++) {
                    int colorIndex = frame->RasterBits[y * src_w + x];
                    GifColorType color = cmap->Colors[colorIndex];
                    int idx = (y * src_w + x) * 3;
                    src_rgb[idx+0] = color.Red;
                    src_rgb[idx+1] = color.Green;
                    src_rgb[idx+2] = color.Blue;
                }
            }

            // Scale to 128x32
            scale_image(src_rgb, src_w, src_h, scaled_rgb);
            // Convert to mono
            rgb_to_mono(scaled_rgb, mono_buf);
            // Draw on OLED
            ssd1306_draw_bitmap(mono_buf);

            free(src_rgb);
            usleep(100000); // 100 ms per frame
        }
    }

    DGifCloseFile(gif, &err);
    close(i2c_fd);
    return 0;
}

