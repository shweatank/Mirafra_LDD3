#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <SDL2/SDL.h>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define PIX_FMT V4L2_PIX_FMT_GREY
#define CAPTURE_FILE "capture.GREY"
#define CAPTURE_SECONDS 10

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    // Set format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = PIX_FMT;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("VIDIOC_S_FMT"); return 1; }

    // Capture
    FILE *f = fopen(CAPTURE_FILE, "wb");
    if (!f) { perror("fopen"); return 1; }

    size_t frame_size = WIDTH * HEIGHT * 2; // Y10 = 2 bytes per pixel
    unsigned char *frame_buf = malloc(frame_size);
    if (!frame_buf) { perror("malloc"); return 1; }

    int fps = 30; // approximate wait between frames
    for (int i = 0; i < CAPTURE_SECONDS * fps; i++) {
        if (read(fd, frame_buf, frame_size) != frame_size) {
            perror("read");
            break;
        }
        fwrite(frame_buf, 1, frame_size, f);
    }
    fclose(f);
    close(fd);

    printf("Capture done. Playing back...\n");

    // Playback with SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    SDL_Window *window = SDL_CreateWindow("Playback", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    FILE *play_file = fopen(CAPTURE_FILE, "rb");
    if (!play_file) { perror("fopen"); return 1; }

    while (fread(frame_buf, 1, frame_size, play_file) == frame_size) {
        uint16_t *src = (uint16_t *)frame_buf;
        uint16_t rgb_buf[WIDTH*HEIGHT];
        for (int i = 0; i < WIDTH*HEIGHT; i++) {
            uint16_t val = src[i] >> 6; // 10-bit to 4-bit
            rgb_buf[i] = (val << 10) | (val << 5) | val; // RGB555
        }
        SDL_UpdateTexture(texture, NULL, rgb_buf, WIDTH*2);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(33); // ~30 FPS
    }

    fclose(play_file);
    free(frame_buf);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

