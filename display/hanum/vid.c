#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define LCD_DEVICE "/dev/ili9225"
#define WIDTH  176
#define HEIGHT 220

// Function to display one frame
int display_rgb565_frame(const char *filename, int fd)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return -1;
    }

    size_t frame_size = WIDTH * HEIGHT * 2; // RGB565 = 2 bytes/pixel
    uint8_t *buf = malloc(frame_size);
    if (!buf) {
        perror("malloc");
        fclose(f);
        return -1;
    }

    size_t read_bytes = fread(buf, 1, frame_size, f);
    fclose(f);

    if (read_bytes != frame_size) {
        fprintf(stderr, "Expected %zu bytes, got %zu: %s\n", frame_size, read_bytes, filename);
        free(buf);
        return -1;
    }

    ssize_t written = write(fd, buf, frame_size);
    if (written != frame_size) {
        perror("write");
        free(buf);
        return -1;
    }

    free(buf);
    return 0;
}

int main()
{
    int fd = open(LCD_DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("open LCD device");
        return 1;
    }

    const int FRAME_DELAY_US =12000; //54000; // ~30 FPS
while(1){
    for (int i = 1; i <=10886; i += 1) {
        char name[128];
        snprintf(name, sizeof(name), "photo-%d.rgb", i);
        if (display_rgb565_frame(name, fd) == 0) {
             usleep(FRAME_DELAY_US); // control frame rate
        } else {
            fprintf(stderr, "Skipping frame %d\n", i);
        }
    }
}
    close(fd);
    return 0;
}

