// image2c_full.c
// Compile: gcc image2c_full.c -o image2c_full -lm
// Requires: stb_image.h and stb_image_resize.h in same folder.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

/* ---------- Helpers ---------- */

static void to_grayscale_from_rgba(uint8_t *rgba, int iw, int ih, uint8_t *gray, int white_bg_if_transparent) {
    for (int y = 0; y < ih; ++y) {
        for (int x = 0; x < iw; ++x) {
            int i = (y * iw + x) * 4;
            uint8_t r = rgba[i + 0], g = rgba[i + 1], b = rgba[i + 2], a = rgba[i + 3];
            if (a < 10 && white_bg_if_transparent) { // transparent -> white
                gray[y * iw + x] = 255;
            } else {
                // alpha compositing over white background if partially transparent
                if (a < 255) {
                    float af = a / 255.0f;
                    float rf = r * af + 255.0f * (1.0f - af);
                    float gf = g * af + 255.0f * (1.0f - af);
                    float bf = b * af + 255.0f * (1.0f - af);
                    gray[y * iw + x] = (uint8_t)(0.299f * rf + 0.587f * gf + 0.114f * bf + 0.5f);
                } else {
                    gray[y * iw + x] = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b + 0.5f);
                }
            }
        }
    }
}

// Simple nearest-neighbor resize for grayscale (fallback)
static void resize_gray_nearest(const uint8_t *in, int iw, int ih, uint8_t *out, int ow, int oh) {
    for (int y = 0; y < oh; ++y) {
        for (int x = 0; x < ow; ++x) {
            int sx = x * iw / ow;
            int sy = y * ih / oh;
            out[y * ow + x] = in[sy * iw + sx];
        }
    }
}

// Floyd–Steinberg dithering (in-place on grayscale buffer, result will be 0 or 255)
static void floyd_steinberg(uint8_t *img, int w, int h, int threshold) {
    // operate on float buffer for error distribution
    float *f = malloc(w * h * sizeof(float));
    if (!f) return;
    for (int i = 0; i < w * h; ++i) f[i] = img[i];

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = y * w + x;
            float old = f[idx];
            float newv = (old > threshold) ? 255.0f : 0.0f;
            float err = old - newv;
            f[idx] = newv;
            if (x + 1 < w) f[idx + 1] += err * 7.0f / 16.0f;
            if (x - 1 >= 0 && y + 1 < h) f[idx + w - 1] += err * 3.0f / 16.0f;
            if (y + 1 < h) f[idx + w] += err * 5.0f / 16.0f;
            if (x + 1 < w && y + 1 < h) f[idx + w + 1] += err * 1.0f / 16.0f;
        }
    }
    for (int i = 0; i < w * h; ++i) img[i] = (uint8_t) (f[i] < 128.0f ? 0 : 255);
    free(f);
}

// rotate 90 cw, 180, 270 cw. Source and dest buffers may be same (we allocate dest).
static uint8_t *rotate_gray(const uint8_t *in, int w, int h, int angle, int *out_w, int *out_h) {
    if (angle == 0) {
        uint8_t *out = malloc(w * h);
        memcpy(out, in, w * h);
        *out_w = w; *out_h = h;
        return out;
    }
    if (angle == 180) {
        uint8_t *out = malloc(w * h);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                out[(h - 1 - y) * w + (w - 1 - x)] = in[y * w + x];
        *out_w = w; *out_h = h;
        return out;
    }
    if (angle == 90) {
        uint8_t *out = malloc(w * h);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                out[x * h + (h - 1 - y)] = in[y * w + x];
        *out_w = h; *out_h = w;
        return out;
    }
    if (angle == 270) {
        uint8_t *out = malloc(w * h);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                out[(w - 1 - x) * h + y] = in[y * w + x];
        *out_w = h; *out_h = w;
        return out;
    }
    return NULL;
}

// flip horizontal/vertical (in-place)
static void flip_gray(uint8_t *img, int w, int h, int flip_h, int flip_v) {
    if (flip_h) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w/2; ++x) {
                uint8_t tmp = img[y*w + x];
                img[y*w + x] = img[y*w + (w - 1 - x)];
                img[y*w + (w - 1 - x)] = tmp;
            }
        }
    }
    if (flip_v) {
        for (int y = 0; y < h/2; ++y) {
            for (int x = 0; x < w; ++x) {
                uint8_t tmp = img[y*w + x];
                img[y*w + x] = img[(h - 1 - y)*w + x];
                img[(h - 1 - y)*w + x] = tmp;
            }
        }
    }
}

/* ---------- Packing and output ---------- */

// horizontal mode: pack row-by-row, 8 pixels left->right per byte, MSB = leftmost bit. black=1 white=0
static void output_horizontal(FILE *f, const uint8_t *img, int w, int h, int invert_bits) {
    fprintf(f, "// %dx%d horizontal, 1-bit per pixel (plain bytes)\n", w, h);
    for (int y = 0; y < h; ++y) {
        for (int xb = 0; xb < (w + 7)/8; ++xb) {
            uint8_t b = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int x = xb*8 + bit;
                if (x >= w) continue;
                uint8_t pixel = img[y*w + x]; // 0..255, 0=black? we will interpret below
                int bitval = (pixel < 128) ? 1 : 0; // black => 1
                if (invert_bits) bitval = !bitval;
                if (bitval) b |= (1 << (7 - bit)); // MSB-first
            }
            fprintf(f, "0x%02X, ", b);
        }
        fprintf(f, "\n");
    }
}

// vertical (SSD1306 page) mode: for each page (8 rows) then x across
// Each byte = vertical 8 pixels (LSB is top pixel in many SSD1306 libs). We'll use bit (1<<bit), bit=0 top.
static void output_vertical(FILE *f, const uint8_t *img, int w, int h, int invert_bits) {
    fprintf(f, "// %dx%d vertical (SSD1306 page) 1-bit per pixel\n", w, h);
    int pages = (h + 7)/8;
    for (int p = 0; p < pages; ++p) {
        for (int x = 0; x < w; ++x) {
            uint8_t b = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int y = p*8 + bit;
                if (y >= h) continue;
                uint8_t pixel = img[y*w + x]; // 0..255
                int bitval = (pixel < 128) ? 1 : 0;
                if (invert_bits) bitval = !bitval;
                if (bitval) b |= (1 << bit); // LSB = top
            }
            fprintf(f, "0x%02X, ", b);
        }
        fprintf(f, "\n");
    }
}

/* ---------- Main CLI ---------- */

static int ask_yesno(const char *prompt) {
    char buf[16];
    while (1) {
        printf("%s (y/n): ", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;
        if (tolower(buf[0]) == 'y') return 1;
        if (tolower(buf[0]) == 'n') return 0;
    }
}


int main() {
    char infile[512];
    const char *outfile = "all_images.c";
    FILE *f = fopen(outfile, "w");
    if (!f) { perror("fopen"); return 1; }

    int canvas_w = 128, canvas_h = 32;
    int bg_choice = 1; // white background
    int invert = 0;
    int dithering = 0;
    int threshold;
    printf("enter threshold value 0-255\n");
    scanf("%d",&threshold);
    int rotate = 0;
    int flip_h = 0, flip_v = 0;
    int draw_mode = 0;
    int invert_output_bits = 0;
    printf("enetr invert_output_bits -1 or -0");
    scanf("%d",&invert_output_bits);

    // Write file header
    fprintf(f, "// Bulk image bitmap data\n");
    fprintf(f, "#include <stdint.h>\n\n");
    fprintf(f, "const uint8_t images[150][%d] = {\n", (canvas_w * canvas_h) / 8);

    for (int img_num = 1; img_num <= 150; img_num++) {
	    if(img_num <10)
	    {
        	sprintf(infile, "output_resized-[00%d].png", img_num);
	    }
	    else if(img_num >=10 && img_num<100)
	    {
		    sprintf(infile, "output_resized-[0%d].png", img_num);
	    }
	    else if(img_num >=100)
	    {
		    sprintf(infile, "output_resized-[%d].png", img_num);
	    }

        int iw, ih, ic;
        uint8_t *rgba = stbi_load(infile, &iw, &ih, &ic, 4);
        if (!rgba) {
            fprintf(stderr, "Failed to load %s\n", infile);
            // Optionally write empty entry
            uint8_t zero[(32*32)/8] = {0};
            fprintf(f, "{");
            for (int i = 0; i < (32*32)/8; i++) fprintf(f, "0x%02X,", zero[i]);
            fprintf(f, "},\n");
            continue;
        }

        int white_bg = (bg_choice == 1) ? 1 : 0;
        uint8_t *gray_src = malloc(iw * ih);
        to_grayscale_from_rgba(rgba, iw, ih, gray_src, white_bg);
        stbi_image_free(rgba);

        uint8_t *resized = malloc(canvas_w * canvas_h);
        if (!stbir_resize_uint8(gray_src, iw, ih, 0, resized, canvas_w, canvas_h, 0, 1)) {
            resize_gray_nearest(gray_src, iw, ih, resized, canvas_w, canvas_h);
        }
        free(gray_src);

        if (invert) {
            for (int i = 0; i < canvas_w * canvas_h; ++i)
                resized[i] = 255 - resized[i];
        }

        if (dithering == 1) {
            floyd_steinberg(resized, canvas_w, canvas_h, threshold);
        } else {
            for (int i = 0; i < canvas_w * canvas_h; ++i)
                resized[i] = (resized[i] > threshold) ? 255 : 0;
        }

        int cur_w = canvas_w, cur_h = canvas_h;
        uint8_t *rotated = rotate_gray(resized, canvas_w, canvas_h, rotate, &cur_w, &cur_h);
        free(resized);

        if (flip_h || flip_v)
            flip_gray(rotated, cur_w, cur_h, flip_h, flip_v);

        // Start array for this image
        fprintf(f, "{");
        if (draw_mode == 0) {
            for (int y = 0; y < cur_h; ++y) {
                for (int xb = 0; xb < (cur_w + 7) / 8; ++xb) {
                    uint8_t b = 0;
                    for (int bit = 0; bit < 8; ++bit) {
                        int x = xb * 8 + bit;
                        if (x >= cur_w) continue;
                        uint8_t pixel = rotated[y * cur_w + x];
                        int bitval = (pixel < 128) ? 1 : 0;
                        if (invert_output_bits) bitval = !bitval;
                        if (bitval) b |= (1 << (7 - bit));
                    }
                    fprintf(f, "0x%02X,", b);
                }
            }
        }
        fprintf(f, "},\n");

        free(rotated);
        printf("Processed %s\n", infile);
    }

    fprintf(f, "};\n");
    fclose(f);

    printf("Done. Output saved to %s\n", outfile);
    return 0;
}

