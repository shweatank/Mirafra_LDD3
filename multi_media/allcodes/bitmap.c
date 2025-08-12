// bitmap_tool_full.c
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*uint8_t *load_and_resize(const char *path, int *w, int *h) {
    int iw, ih, ic;
    uint8_t *img = stbi_load(path, &iw, &ih, &ic, 1);
    if (!img) return NULL;
    uint8_t *res = malloc((*w) * (*h));
    stbir_resize_uint8(img, iw, ih, 0, res, *w, *h, 0, 1);
    stbi_image_free(img);
    return res;
}*/
uint8_t *load_and_resize(const char *path, int *w, int *h) {
    int iw, ih, ic;
    uint8_t *img = stbi_load(path, &iw, &ih, &ic, 4); // RGBA
    if (!img) return NULL;

    uint8_t *gray = malloc(iw * ih);
    for (int i = 0; i < iw * ih; i++) {
        uint8_t r = img[i*4 + 0];
        uint8_t g = img[i*4 + 1];
        uint8_t b = img[i*4 + 2];
        uint8_t a = img[i*4 + 3];

        // White background: if alpha < 128, make pixel white
        if (a < 128) gray[i] = 255;
        else gray[i] = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
    }
    free(img);

    uint8_t *res = malloc((*w) * (*h));
    stbir_resize_uint8(gray, iw, ih, 0, res, *w, *h, 0, 1);
    free(gray);
    return res;
}


void invert(uint8_t *img, int sz) {
    for (int i = 0; i < sz; i++) img[i] = 255 - img[i];
}

void rotate90(uint8_t *img, int w, int h, uint8_t *out) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            out[x*h + (h - y - 1)] = img[y*w + x];
}

int main(int argc, char **argv) {
    if (argc < 8) {
        printf("Usage:\n%s in.png out.h width height threshold invert [0/1] rotate [0/90/180/270]\n", argv[0]);
        return 1;
    }

    const char *in = argv[1], *out = argv[2];
    int W = atoi(argv[3]), H = atoi(argv[4]);
    int thresh = atoi(argv[5]);
    int inv = atoi(argv[6]);
    int rot = atoi(argv[7]);

    uint8_t *img = load_and_resize(in, &W, &H);
    if (!img) { printf("Load failed\n"); return 1; }
    int sz = W * H;
    if (inv) invert(img, sz);

    // threshold
    for (int i = 0; i < sz; i++)
        img[i] = img[i] > thresh ? 255 : 0;

    // rotate
    if (rot == 90 || rot == 270) {
        uint8_t *tmp = malloc(sz);
        rotate90(img, W, H, tmp);
        free(img);
        img = tmp;
        int tmpv = W; W = H; H = tmpv;
        sz = W * H;
    }

    FILE *f = fopen(out, "w");
    fprintf(f, "// '%s', %dx%dpx\n", out, W, H);

    for (int y = 0; y < H; y++) {
        for (int xb = 0; xb < W/8; xb++) {
            uint8_t b = 0;
            for (int bit = 0; bit < 8; bit++) {
                if (img[y*W + xb*8 + bit] == 0)
                    b |= (1 << (7 - bit));
            }
            fprintf(f, "0x%02X, ", b);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    free(img);
    printf("Saved to %s\n", out);
    return 0;
}

