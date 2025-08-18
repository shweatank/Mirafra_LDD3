// gcc -O2 recv_display_oled.c -o recv_display_oled -ljpeg
// sudo ./recv_display_oled /dev/serial0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
//#include <linux/i2c-dev.h>
//#include <jpeglib.h>
#include<stdint.h>
#include <sys/ioctl.h>       // <-- for ioctl()
#include <linux/i2c-dev.h>   // <-- for I2C_SLAVE
#include <jpeglib.h>         // <-- for JPEG decoding

#define OLED_ADDR 0x3C
#define OLED_W 128
#define OLED_H 64

int uart_init(const char *dev, speed_t baud) {
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) return -1;
    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);
    tty.c_iflag = tty.c_oflag = tty.c_lflag = 0;
    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

int i2c_cmd(int fd, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    return write(fd, buf, 2);
}

int oled_init(int fd) {
    uint8_t cmds[] = {0xAE,0x20,0x00,0xB0,0xC8,0x00,0x10,0x40,0x81,0x7F,
                      0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,0xD9,
                      0x22,0xDA,0x12,0xDB,0x20,0x8D,0x14,0xAF};
    for (size_t i=0;i<sizeof(cmds);i++) i2c_cmd(fd,cmds[i]);
    return 0;
}

void oled_data(int fd, uint8_t *data, size_t len) {
    uint8_t buf[17];
    for (size_t i=0;i<len;i+=16) {
        buf[0]=0x40;
        size_t chunk=(len-i>16)?16:len-i;
        memcpy(buf+1,data+i,chunk);
        write(fd,buf,chunk+1);
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
    int width = cinfo.output_width, height = cinfo.output_height;
    int pixel_size = cinfo.output_components;
    unsigned char *bmp = malloc(width*height*pixel_size);
    while (cinfo.output_scanline < height) {
        unsigned char *rowptr[1];
        rowptr[0] = bmp + cinfo.output_scanline * width * pixel_size;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    *w = width; *h = height;
    return bmp;
}

void rgb_to_oled(unsigned char *rgb, int w, int h, uint8_t *dst) {
    memset(dst, 0, OLED_W * (OLED_H/8));
    for (int y=0;y<OLED_H;y++) {
        int sy = y * h / OLED_H;
        for (int x=0;x<OLED_W;x++) {
            int sx = x * w / OLED_W;
            int idx = (sy*w + sx) * 3;
            int lum = (rgb[idx]*299 + rgb[idx+1]*587 + rgb[idx+2]*114) / 1000;
            if (lum > 128) dst[(y/8)*OLED_W + x] |= (1<<(y%8));
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr,"Usage: %s <uart_dev>\n",argv[0]); return 1; }

    int uart_fd = uart_init(argv[1], B115200);
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    ioctl(i2c_fd, I2C_SLAVE, OLED_ADDR);
    oled_init(i2c_fd);

    uint32_t size;
    read(uart_fd, &size, sizeof(size));

    unsigned char *jpeg_buf = malloc(size);
    size_t recvd = 0;
    while (recvd < size) {
        ssize_t r = read(uart_fd, jpeg_buf+recvd, size-recvd);
        if (r > 0) recvd += r;
    }
    printf("jpeg buf:%s",jpeg_buf);
    int w,h;
    unsigned char *rgb = jpeg_to_rgb(jpeg_buf, size, &w, &h);

    uint8_t oled_buf[OLED_W*(OLED_H/8)];
    rgb_to_oled(rgb, w, h, oled_buf);
    oled_data(i2c_fd, oled_buf, sizeof(oled_buf));

    free(rgb);
    free(jpeg_buf);
    close(i2c_fd);
    close(uart_fd);
    return 0;
}

