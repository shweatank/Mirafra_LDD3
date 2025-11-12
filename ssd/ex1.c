#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    int16_t width;
    int16_t height;
    uint8_t buffer[(128*32)/8];
} oled;

int main() {
    int fd = open("/dev/ssd1306_fb", O_WRONLY);
    if (fd < 0) { perror("open"); return 1; }

    oled img = {64, 32, {0}};
    // Fill img.buffer with test pattern
    for (int i = 0; i < sizeof(img.buffer); i++)
        img.buffer[i] = 0xFF;

    write(fd, &img, sizeof(img));
    close(fd);
    return 0;
}

