// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <jpeglib.h>

#define OLED_ADDR 0x3C
#define OLED_W 128
#define OLED_H 64

int oled_cmd(int fd, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    return write(fd, buf, 2);
}

void oled_init(int fd) {
    uint8_t cmds[] = {
        0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,0x81,0x7F,
        0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,0xD9,
        0x22,0xDA,0x12,0xDB,0x20,0x8D,0x14,0xAF
    };
    for (size_t i = 0; i < sizeof(cmds); i++)
        oled_cmd(fd, cmds[i]);
}

void oled_data(int fd, uint8_t *data, size_t len) {
    uint8_t buf[17];
    for (size_t i = 0; i < len; i += 16) {
        buf[0] = 0x40;
        size_t chunk = (len - i > 16) ? 16 : len - i;
        memcpy(buf + 1, data + i, chunk);
        write(fd, buf, chunk + 1);
    }
}

unsigned char* jpeg_to_rgb(unsigned char *mem, size_t memlen, int *w, int *h) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, mem, memlen);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int pixel_size = cinfo.output_components;

    unsigned char *bmp = malloc(width * height * pixel_size);
    if (!bmp) {
        fprintf(stderr, "Failed to allocate memory for image\n");
        return NULL;
    }

    while (cinfo.output_scanline < height) {
        unsigned char *rowptr[1];
        rowptr[0] = bmp + cinfo.output_scanline * width * pixel_size;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    *w = width;
    *h = height;
    return bmp;
}

void rgb_to_oled(unsigned char *rgb, int w, int h, uint8_t *dst) {
    memset(dst, 0, OLED_W * (OLED_H / 8));
    for (int y = 0; y < OLED_H; y++) {
        int sy = y * h / OLED_H;
        for (int x = 0; x < OLED_W; x++) {
            int sx = x * w / OLED_W;
            int idx = (sy * w + sx) * 3;
            int lum = (rgb[idx] * 299 + rgb[idx + 1] * 587 + rgb[idx + 2] * 114) / 1000;
            if (lum > 128)
                dst[(y / 8) * OLED_W + x] |= (1 << (y % 8));
        }
    }
}

int main(int argc, char **argv) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return 1;
    }
    printf("Waiting for connection...\n");

    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }
    printf("Client connected\n");

    // Open OLED
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        perror("I2C open");
        return 1;
    }
    ioctl(i2c_fd, I2C_SLAVE, OLED_ADDR);
    oled_init(i2c_fd);

    uint8_t oled_buf[OLED_W * (OLED_H / 8)];

    while (1) {
        uint32_t size_net;
        ssize_t r = read(client_fd, &size_net, sizeof(size_net));
        if (r <= 0) {
            printf("Connection closed or error\n");
            break;
        }
        uint32_t size = ntohl(size_net);
        if (size > 100000) {
            fprintf(stderr, "Frame too large\n");
            break;
        }

        unsigned char *jpeg_buf = malloc(size);
        if (!jpeg_buf) {
            fprintf(stderr, "Memory alloc failed\n");
            break;
        }

        size_t recvd = 0;
        while (recvd < size) {
            r = read(client_fd, jpeg_buf + recvd, size - recvd);
            if (r <= 0) {
                fprintf(stderr, "Read error\n");
                free(jpeg_buf);
                goto done;
            }
            recvd += r;
        }

        int w, h;
        unsigned char *rgb = jpeg_to_rgb(jpeg_buf, size, &w, &h);
        free(jpeg_buf);
        if (!rgb) {
            fprintf(stderr, "JPEG decode failed\n");
            continue;
        }

        rgb_to_oled(rgb, w, h, oled_buf);
        oled_data(i2c_fd, oled_buf, sizeof(oled_buf));
        free(rgb);
    }

done:
    close(client_fd);
    close(server_fd);
    close(i2c_fd);
    return 0;
}

