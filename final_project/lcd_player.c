// ili9225_player.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define DEVICE "/dev/ili9225"
#define WIDTH 176
#define HEIGHT 220
#define FRAME_SIZE (WIDTH * HEIGHT * 2)  // RGB565: 2 bytes per pixel
#define NUM_FRAMES 100
#define FRAME_DELAY_US 100000            // 100 ms per frame (10 FPS)

long timespec_diff_us(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000000L +
           (end->tv_nsec - start->tv_nsec) / 1000L;
}

int main(void)
{
    int fd;
    char filename[256];
    unsigned char *buffer;
    FILE *fp;
    int i;

    fd = open(DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open " DEVICE);
        return 1;
    }

    buffer = malloc(FRAME_SIZE);
    if (!buffer) {
        perror("malloc");
        close(fd);
        return 1;
    }

    printf("Starting playback of %d frames at ~10 FPS...\n", NUM_FRAMES);

    for (i = 1; i <= NUM_FRAMES; i++) {
        struct timespec ts_start, ts_end;
        long elapsed_us, sleep_us;

        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        snprintf(filename, sizeof(filename), "frames/photo-%03d.rgb", i);
        fp = fopen(filename, "rb");
        if (!fp) {
            fprintf(stderr, "Error opening %s: %s\n", filename, strerror(errno));
            break;
        }

        if (fread(buffer, 1, FRAME_SIZE, fp) != FRAME_SIZE) {
            fprintf(stderr, "Error reading %s\n", filename);
            fclose(fp);
            break;
        }
        fclose(fp);

        if (write(fd, buffer, FRAME_SIZE) != FRAME_SIZE) {
            perror("write");
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);
        elapsed_us = timespec_diff_us(&ts_start, &ts_end);
        sleep_us = FRAME_DELAY_US - elapsed_us;
        if (sleep_us > 0) usleep(sleep_us);

        // Optional: print FPS info every 10 frames
        if (i % 10 == 0) {
            printf("Frame %d/%d displayed\n", i, NUM_FRAMES);
        }
    }

    free(buffer);
    close(fd);
    printf("Playback finished.\n");
    return 0;
}

