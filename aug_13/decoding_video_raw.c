// Build:
//   gcc video_decode.c -o decode_h265 $(pkg-config --cflags --libs libde265)
// Run:
//   ./decode_h265 input.h265 output.yuv
//
// View (example; use the size printed by the program):
//   ffplay -f rawvideo -pixel_format yuv420p -video_size 1920x1080 output.yuv

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libde265/de265.h>

static uint8_t* read_file(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) { perror("fopen"); return NULL; }

    if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) { perror("ftell"); fclose(f); return NULL; }
    rewind(f);

    uint8_t* buf = (uint8_t*)malloc((size_t)sz);
    if (!buf) { fprintf(stderr, "malloc failed\n"); fclose(f); return NULL; }

    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { fprintf(stderr, "short read\n"); free(buf); return NULL; }

    *out_size = (size_t)sz;
    return buf;
}

static void write_plane(FILE* out, const uint8_t* src, int stride, int width, int height) {
    // Write plane row by row, respecting stride.
    for (int y = 0; y < height; ++y) {
        fwrite(src + y * stride, 1, (size_t)width, out);
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.h265> <output.yuv>\n", argv[0]);
        return 1;
    }

    const char* in_path  = argv[1];
    const char* out_path = argv[2];

    size_t filesize = 0;
    uint8_t* bitstream = read_file(in_path, &filesize);
    if (!bitstream) return 1;

    FILE* out = fopen(out_path, "wb");
    if (!out) {
        perror("fopen output");
        free(bitstream);
        return 1;
    }

    de265_decoder_context* ctx = de265_new_decoder();
    if (!ctx) {
        fprintf(stderr, "de265_new_decoder failed\n");
        fclose(out);
        free(bitstream);
        return 1;
    }

    // libde265 v1.0.15 signature:
    // de265_push_data(ctx, data, length, de265_PTS pts, void* user_data)
    de265_error err = de265_push_data(ctx, bitstream, (int)filesize, 0 /*pts*/, NULL /*user_data*/);
    if (err != DE265_OK) {
        fprintf(stderr, "de265_push_data error: %s\n", de265_get_error_text(err));
        de265_free_decoder(ctx);
        fclose(out);
        free(bitstream);
        return 1;
    }

    int printed_dims = 0;
    int more = 1;
    while (more) {
        err = de265_decode(ctx, &more);
        if (err != DE265_OK && err != DE265_ERROR_WAITING_FOR_INPUT_DATA) {
            fprintf(stderr, "de265_decode error: %s\n", de265_get_error_text(err));
            break;
        }

        const struct de265_image* img = NULL;
        while ((img = de265_get_next_picture(ctx)) != NULL) {
            int w = de265_get_image_width(img, 0);
            int h = de265_get_image_height(img, 0);
            if (!printed_dims) {
                fprintf(stderr, "Decoded stream resolution: %dx%d\n", w, h);
                fprintf(stderr, "Tip: ffplay -f rawvideo -pixel_format yuv420p -video_size %dx%d %s\n",
                        w, h, out_path);
                printed_dims = 1;
            }

            // Ensure it's 4:2:0 (I420). If not, warn but still try (you may need conversion).
            int chroma = de265_get_chroma_format(img); // 0:400, 1:420, 2:422, 3:444 (values per libde265)
            if (chroma != 1) {
                fprintf(stderr, "Warning: chroma format is not 4:2:0 (value=%d). Output expects I420.\n", chroma);
            }

            // Fetch planes with stride
            int strideY = 0, strideU = 0, strideV = 0;
            const uint8_t* pY = (const uint8_t*)de265_get_image_plane(img, 0, &strideY);
            const uint8_t* pU = (const uint8_t*)de265_get_image_plane(img, 1, &strideU);
            const uint8_t* pV = (const uint8_t*)de265_get_image_plane(img, 2, &strideV);

            if (!pY || !pU || !pV) {
                fprintf(stderr, "Error: missing image plane(s).\n");
                continue;
            }

            // Write I420 layout: Y (w*h), then U (w/2*h/2), then V (w/2*h/2)
            write_plane(out, pY, strideY, w, h);
            write_plane(out, pU, strideU, w/2, h/2);
            write_plane(out, pV, strideV, w/2, h/2);
        }
    }

    de265_flush_data(ctx);
    de265_free_decoder(ctx);
    fclose(out);
    free(bitstream);

    fprintf(stderr, "Done. Wrote raw YUV420p to: %s\n", out_path);
    return 0;
}

