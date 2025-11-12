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


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s video.mp4\n", argv[0]);
        return 1;
    }


    int fb = open(DEVICE, O_WRONLY);
    if (fb < 0) {
        perror("open device");
        return 1;
    }

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "ffmpeg -nostdin -i \"%s\" -vf scale=%d:%d -f rawvideo -pix_fmt rgb565 -vsync 0 -",
             argv[1], WIDTH, HEIGHT);

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


    while (fread(buffer, 1, FRAME_SIZE, pipe) == FRAME_SIZE) {

        if (write(fb, buffer, FRAME_SIZE) < 0) {
            perror("write");
            break;
        }
    }

    free(buffer);
    pclose(pipe);
    close(fb);

    return 0;
}

