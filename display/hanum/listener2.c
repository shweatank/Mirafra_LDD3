#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define WIDTH   176
#define HEIGHT  220
#define BPP     2
#define FRAME_SIZE (WIDTH * HEIGHT * BPP)

int main() {
    int lcd_fd = open("/dev/ili9225", O_WRONLY);
    if (lcd_fd < 0) {
        perror("open /dev/ili9225");
        return 1;
    }

    // FFmpeg command to listen on TCP and output raw RGB565 frames
    const char *cmd =
	    "ffmpeg -fflags nobuffer -flags low_delay -i tcp://192.168.0.165:5000 \
-an -vf scale=176:220 -pix_fmt rgb565le -f rawvideo -r 24 -";
	    /*"ffmpeg -nostdin  -fflags nobuffer "
"-i tcp://192.168.0.165:5000 "
"-an "
"-vf scale=176:220 "
"-pix_fmt rgb565le "
"-f rawvideo "
"-r 24 -";*/

    FILE *ffmpeg = popen(cmd, "r");
    if (!ffmpeg) {
        perror("popen ffmpeg");
        close(lcd_fd);
        return 1;
    }

    unsigned char buffer[FRAME_SIZE];
    while (1) {
        size_t bytes = fread(buffer, 1, FRAME_SIZE, ffmpeg);
        if (bytes < FRAME_SIZE) {
            if (feof(ffmpeg)) {
                fprintf(stderr, "Stream ended\n");
            } else {
                perror("fread");
            }
            break;
        }

        if (write(lcd_fd, buffer, FRAME_SIZE) < 0) {
            perror("write lcd");
            break;
        }

        // Optional: sleep to maintain ~24fps
        usleep(1000000 / 24);
    }

    pclose(ffmpeg);
    close(lcd_fd);
    return 0;
}

