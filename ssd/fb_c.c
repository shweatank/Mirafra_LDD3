#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

int main() {
    int fb = open("/dev/fb0", O_RDWR);
    if (fb < 0) {
        perror("open /dev/fb0");
        return 1;
    }

    // Clear framebuffer
    struct fb_var_screeninfo vinfo;
    if (ioctl(fb, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("ioctl FBIOGET_VSCREENINFO");
        close(fb);
        return 1;
    }

    int fb_size = vinfo.xres * vinfo.yres / 8;
    unsigned char *buf = malloc(fb_size);
    if (!buf) {
        perror("malloc");
        close(fb);
        return 1;
    }

    // Draw a simple pattern: alternating bytes 0xAA / 0x55
    for (int i = 0; i < fb_size; i++) {
        buf[i] = (i % 2) ? 0xAA : 0x55;
    }

    if (write(fb, buf, fb_size) != fb_size) {
        perror("write");
    }

    free(buf);
    close(fb);
    printf("Pattern written to framebuffer\n");
    return 0;
}
