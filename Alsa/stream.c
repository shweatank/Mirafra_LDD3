#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define WIDTH   176
#define HEIGHT  220
#define BPP     2   // bytes per pixel in RGB565
#define FRAME_SIZE (WIDTH * HEIGHT * BPP)

int main() {
    int lcd_fd = open("/dev/ili9225", O_WRONLY);
    if (lcd_fd < 0) {
        perror("open /dev/ili9225");
        return 1;
    }

    // Run ffmpeg as a pipe
    FILE *ffmpeg = popen(
        "ffmpeg -nostdin -i hanuman_chalisa.mp4 "
        "-vf scale=176:220 "
        "-f rawvideo -pix_fmt rgb565le -r 24 -",
        "r"
    );
    if (!ffmpeg) {
        perror("popen ffmpeg");
        close(lcd_fd);
        return 1;
    }

    unsigned char buffer[FRAME_SIZE];
    while (1) {
        size_t bytes = fread(buffer, 1, FRAME_SIZE, ffmpeg);
        if (bytes < FRAME_SIZE) {
            break;  // End of stream
        }

        ssize_t written = write(lcd_fd, buffer, FRAME_SIZE);
        if (written < 0) {
            perror("write frame to lcd");
            break;
        }

        // Optional: small delay to match fps
        usleep(1000000 / 24); // 24 fps
    }

    pclose(ffmpeg);
    close(lcd_fd);
    return 0;
}

