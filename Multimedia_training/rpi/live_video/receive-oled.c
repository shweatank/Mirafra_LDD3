#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define OUT_W 128
#define OUT_H 32
#define OUT_BYTES ((OUT_W * OUT_H) / 8) //512

static uint16_t crc16(const uint8_t *data, size_t n) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < n; ++i) {
        crc ^= data[i];
        for (int k=0;k<8;k++) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

static ssize_t read_n(int fd, uint8_t *buf, size_t n) {
    size_t left = n; uint8_t *p = buf;
    while (left) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) return (ssize_t)(n - left);
        left -= r; p += r;
    }
    return (ssize_t)n;
}

static int set_serial(int fd, int baud) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) { perror("tcgetattr"); return -1; }
    cfmakeraw(&tty);
    speed_t sp = B115200;
    if (baud == 115200) sp = B115200;
#ifdef B230400
    else if (baud == 230400) sp = B230400;
#endif
#ifdef B460800
    else if (baud == 460800) sp = B460800;
#endif
#ifdef B921600
    else if (baud == 921600) sp = B921600;
#endif
    cfsetospeed(&tty, sp); cfsetispeed(&tty, sp);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE; tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB; tty.c_cflag &= ~CSTOPB;
#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;
#endif
    tty.c_cc[VMIN] = 1; tty.c_cc[VTIME] = 1;
    if (tcsetattr(fd, TCSANOW, &tty) < 0) { perror("tcsetattr"); return -1; }
    return 0;
}

static int i2c_write_cmd(int i2cfd, uint8_t cmd) {
    uint8_t b[2] = {0x00, cmd};
    return write(i2cfd, b, 2) == 2 ? 0 : -1;
}

static int ssd1306_init(int i2cfd) {
    // 128x32 init
    i2c_write_cmd(i2cfd, 0xAE); // display off
    i2c_write_cmd(i2cfd, 0xA8); i2c_write_cmd(i2cfd, 0x1F); // multiplex 32
    i2c_write_cmd(i2cfd, 0xD3); i2c_write_cmd(i2cfd, 0x00);
    i2c_write_cmd(i2cfd, 0x40);
    i2c_write_cmd(i2cfd, 0xA1);
    i2c_write_cmd(i2cfd, 0xC8);
    i2c_write_cmd(i2cfd, 0xDA); i2c_write_cmd(i2cfd, 0x02);
    i2c_write_cmd(i2cfd, 0x81); i2c_write_cmd(i2cfd, 0x7F);
    i2c_write_cmd(i2cfd, 0xA4);
    i2c_write_cmd(i2cfd, 0xA6);
    i2c_write_cmd(i2cfd, 0xD5); i2c_write_cmd(i2cfd, 0x80);
    i2c_write_cmd(i2cfd, 0x8D); i2c_write_cmd(i2cfd, 0x14);
    i2c_write_cmd(i2cfd, 0xAF);
    return 0;
}

static int i2c_write_data(int i2cfd, const uint8_t *data, size_t len) {
    // send in chunks: control byte 0x40 then up to 16 bytes per write to be safe
    size_t off = 0;
    while (off < len) {
        size_t chunk = (len - off) > 16 ? 16 : (len - off);
        uint8_t buf[17];
        buf[0] = 0x40;
        memcpy(buf+1, data+off, chunk);
        if (write(i2cfd, buf, chunk+1) != (ssize_t)(chunk+1)) return -1;
        off += chunk;
    }
    return 0;
}

static int ssd1306_draw_frame(int i2cfd, const uint8_t *frame) {
    uint8_t tmp[OUT_W];
    int pages = OUT_H / 8;
    for (int p=0;p<pages;p++) {
        if (i2c_write_cmd(i2cfd, 0xB0 + p) < 0) return -1;
        if (i2c_write_cmd(i2cfd, 0x00) < 0) return -1;
        if (i2c_write_cmd(i2cfd, 0x10) < 0) return -1;
        // write 128 bytes for this page
        if (i2c_write_data(i2cfd, frame + p*OUT_W, OUT_W) < 0) return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr,"Usage: %s /dev/ttyUSB0 /dev/i2c-1 0x3C [baud]\n", argv[0]);
        return 1;
    }
    const char *serial_dev = argv[1];
    const char *i2c_dev = argv[2];
    int i2c_addr = (int)strtol(argv[3], NULL, 0);
    int baud = 921600;
    if (argc >= 5) baud = atoi(argv[4]);

    int sd = open(serial_dev, O_RDWR | O_NOCTTY);
    if (sd < 0) { perror("open serial"); return 1; }
    if (set_serial(sd, baud) < 0) { fprintf(stderr,"serial set failed\n"); /* continue */ }

    // open i2c
    int i2cfd = open(i2c_dev, O_RDWR);
    if (i2cfd < 0) { perror("open i2c"); return 1; }
    if (ioctl(i2cfd, I2C_SLAVE, i2c_addr) < 0) { perror("I2C_SLAVE"); return 1; }

    ssd1306_init(i2cfd);

    // send initial 'R' so host can start sending
    uint8_t ready = 'R';
    write(sd, &ready, 1);

    // read loop: we find sync then read header/payload/crc
    uint8_t sync[4];
    int pos = 0;
    while (1) {
        uint8_t c;
        ssize_t r = read(sd, &c, 1);
        if (r <= 0) { usleep(1000); continue; }
        sync[pos % 4] = c; pos++;
        if (pos >= 4) {
            if (sync[(pos-4)%4]==0xAA && sync[(pos-3)%4]==0x55 &&
                sync[(pos-2)%4]==0xAA && sync[(pos-1)%4]==0x55) {

                // read seq(4) + len(2)
                uint8_t hdr[6];
                if (read_n(sd, hdr, 6) != 6) { continue; }
                uint32_t seq = (hdr[0]<<24)|(hdr[1]<<16)|(hdr[2]<<8)|hdr[3];
                uint16_t len = (hdr[4]<<8)|hdr[5];
                if (len != OUT_BYTES) {
                    // skip len + crc
                    size_t skip = (size_t)len + 2;
                    uint8_t throw[256];
                    while (skip) {
                        ssize_t rr = read(sd, throw, (skip>sizeof(throw))?sizeof(throw):skip);
                        if (rr <= 0) break;
                        skip -= rr;
                    }
                    // send READY so host can continue (we still have capacity)
                    write(sd, &ready, 1);
                    continue;
                }
                uint8_t payload[OUT_BYTES];
                if (read_n(sd, payload, OUT_BYTES) != OUT_BYTES) { continue; }
                uint8_t crcbytes[2];
                if (read_n(sd, crcbytes, 2) != 2) { continue; }
                uint16_t recvcrc = crcbytes[0] | (crcbytes[1]<<8);

                uint8_t crcbuf[6 + OUT_BYTES];
                memcpy(crcbuf, hdr, 6);
                memcpy(crcbuf + 6, payload, OUT_BYTES);
                uint16_t mycrc = crc16(crcbuf, sizeof(crcbuf));
                if (mycrc != recvcrc) {
                    // drop frame on CRC mismatch
                    // still reply READY to unblock host (optional)
                    write(sd, &ready, 1);
                    continue;
                }

                // draw immediately
                ssd1306_draw_frame(i2cfd, payload);

                // reply READY so host can send next frame (this synchronizes FPS)
                write(sd, &ready, 1);
            }
        }
    }

    close(sd); close(i2cfd);
    return 0;
}

