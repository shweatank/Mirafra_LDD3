#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#define DEVICE "/dev/ili9225"
#define WIDTH  176
#define HEIGHT 220
#define FRAME_SIZE (WIDTH * HEIGHT * 2)  // RGB565 = 2 bytes per pixel

// Returns current time in microseconds
static inline uint64_t current_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

// Get video FPS using ffprobe
double get_fps(const char *filename) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "ffprobe -v 0 -of csv=p=0 -select_streams v:0 -show_entries stream=r_frame_rate \"%s\"",
             filename);

    FILE *fp = popen(cmd, "r");
    if (!fp) return 30.0; // default

    char buf[64];
    if (!fgets(buf, sizeof(buf), fp)) {
        pclose(fp);
        return 30.0;
    }
    pclose(fp);

    // parse fraction like "30000/1001"
    int num = 0, den = 1;
    if (sscanf(buf, "%d/%d", &num, &den) == 2 && den != 0)
        return (double)num / den;
    if (sscanf(buf, "%d", &num) == 1)
        return (double)num;

    return 30.0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s video.mp4\n", argv[0]);
        return 1;
    }

    double fps = get_fps(argv[1]);
    uint64_t frame_interval_us = (uint64_t)(1000000.0 / fps);

    int fb = open(DEVICE, O_WRONLY);
    if (fb < 0) {
        perror("open device");
        return 1;
    }

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "ffmpeg -nostdin -i \"%s\" -vf scale=%d:%d -f rawvideo -pix_fmt rgb565 -r %.2f -vsync 0 -",
             argv[1], WIDTH, HEIGHT, fps);

    FILE *pipe = popen(cmd, "r");
    if (!pipe) {
        perror("popen");
        close(fb);
        return 1;
    }

    unsigned char *buffer = malloc(FRAME_SIZE);
    if (!buffer) {
        perror("malloc");
        pclose(pipe);
        close(fb);
        return 1;
    }

    uint64_t next_frame_time = current_time_us();

    while (fread(buffer, 1, FRAME_SIZE, pipe) == FRAME_SIZE) {
        uint64_t now = current_time_us();
        if (next_frame_time > now)
            usleep(next_frame_time - now);

        if (write(fb, buffer, FRAME_SIZE) < 0) {
            perror("write");
            break;
        }
        next_frame_time += frame_interval_us;
    }

    free(buffer);
    pclose(pipe);
    close(fb);

    return 0;
}

