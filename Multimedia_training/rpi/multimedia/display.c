#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <jpeglib.h>

#define OLED_ADDR 0x3C
#define OLED_W 128
#define OLED_H 64

// Send command byte to OLED
int oled_cmd(int fd, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    return write(fd, buf, 2);
}

// Initialize OLED SSD1306
void oled_init(int fd) {
    uint8_t cmds[] = {
        0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,0x81,0x7F,
        0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,0xD9,
        0x22,0xDA,0x12,0xDB,0x20,0x8D,0x14,0xAF
    };
    for (size_t i = 0; i < sizeof(cmds); i++)
        oled_cmd(fd, cmds[i]);
}

// Send data bytes to OLED (in chunks)
void oled_data(int fd, uint8_t *data, size_t len) {
    uint8_t buf[17];
    for (size_t i = 0; i < len; i += 16) {
        buf[0] = 0x40;  // Data control byte
        size_t chunk = (len - i > 16) ? 16 : len - i;
        memcpy(buf + 1, data + i, chunk);
        write(fd, buf, chunk + 1);
    }
}

// Clear the OLED display by writing all zeros
void oled_clear(int fd) {
    uint8_t clear_buf[OLED_W * (OLED_H / 8)] = {0};
    oled_data(fd, clear_buf, sizeof(clear_buf));
}

// Decode JPEG file to RGB buffer using libjpeg
unsigned char* jpeg_to_rgb(const char *filename, int *w, int *h) {
    FILE *infile = fopen(filename, "rb");
    if (!infile) {
        perror("JPEG open");
        return NULL;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    *w = cinfo.output_width;
    *h = cinfo.output_height;
    int pixel_size = cinfo.output_components;

    unsigned char *bmp = malloc((*w) * (*h) * pixel_size);
    if (!bmp) {
        fprintf(stderr, "Memory allocation failed\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return NULL;
    }

    while (cinfo.output_scanline < *h) {
        unsigned char *rowptr[1];
        rowptr[0] = bmp + cinfo.output_scanline * (*w) * pixel_size;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return bmp;
}

// Convert RGB buffer to 1-bit OLED bitmap buffer with simple scaling
void rgb_to_oled(unsigned char *rgb, int w, int h, uint8_t *dst) {
    memset(dst, 0, OLED_W * (OLED_H / 8));
    for (int y = 0; y < OLED_H; y++) {
        int sy = y * h / OLED_H;
        for (int x = 0; x < OLED_W; x++) {
            int sx = x * w / OLED_W;
            int idx = (sy * w + sx) * 3;
            int lum = (rgb[idx] * 299 + rgb[idx + 1] * 587 + rgb[idx + 2] * 114) / 1000;
            if (lum > 128) {
                dst[(y / 8) * OLED_W + x] |= (1 << (y % 8));
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image.jpg>\n", argv[0]);
        return 1;
    }

    int fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        perror("I2C open failed");
        return 1;
    }
    if (ioctl(fd, I2C_SLAVE, OLED_ADDR) < 0) {
        perror("I2C ioctl failed");
        close(fd);
        return 1;
    }

    oled_init(fd);
    oled_clear(fd);  // Clear before display

    int w, h;
    unsigned char *rgb = jpeg_to_rgb(argv[1], &w, &h);
    if (!rgb) {
        close(fd);
        return 1;
    }

    uint8_t oled_buf[OLED_W * (OLED_H / 8)];
    rgb_to_oled(rgb, w, h, oled_buf);
    oled_data(fd, oled_buf, sizeof(oled_buf));

    free(rgb);
    close(fd);

    return 0;
}

