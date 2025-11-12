#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/ili9225"
#define WIDTH  176
#define HEIGHT 220
#define FRAME_SIZE (WIDTH * HEIGHT * 2)  // RGB565 = 2 bytes per pixel

int main() {

    int fb = open(DEVICE, O_WRONLY);
    if (fb < 0) {
        perror("open device");
        return 1;
    }
while(1){
    FILE *fp = fopen("output.rgb", "rb");
    if (!fp) {
        perror("fopen");
        close(fb);
        return 1;
    }

    unsigned char *buffer = malloc(FRAME_SIZE);
    if (!buffer) {
        perror("malloc");
        fclose(fp);
        close(fb);
        return 1;
    }
    while (fread(buffer, 1, FRAME_SIZE, fp) == FRAME_SIZE) {
        if (write(fb, buffer, FRAME_SIZE) < 0) {
            perror("write");
            break;
        }
        // ~30 FPS (adjust as needed)
       usleep(12000);
    }


    free(buffer);
    fclose(fp);
    close(fb);
}
    return 0;
}

