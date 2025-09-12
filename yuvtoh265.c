// encode_yuyv_i422.c
// Build:   gcc encode_yuyv_i422.c -o encode_yuyv_i422 -lx265
// Input:   raw YUYV422 frames captured at 1280x720 @ 10fps (e.g., from your capture_video.c)
// Output:  raw Annex-B H.265 bitstream (output.h265)

#include <x265.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define WIDTH       1280
#define HEIGHT      720
#define FPS         10
#define INPUT_FILE  "video.yuv"
#define OUTPUT_FILE "output.h265"

// Convert YUYV422 (packed) -> I422 (planar)
static void yuyv_to_i422(const uint8_t *yuyv, uint8_t *y, uint8_t *u, uint8_t *v,
                         int width, int height)
{
    for (int j = 0; j < height; j++) {
        const int row_off_y  = j * width;
        const int row_off_uv = j * (width / 2);
        for (int i = 0; i < width; i += 2) {
            int idx    = (row_off_y + i) * 2;     // bytes in YUYV
            int uv_idx = row_off_uv + (i >> 1);   // U/V index (4:2:2)

            // YUYV byte order: Y0 U Y1 V
            y[row_off_y + i]     = yuyv[idx];       // Y0
            y[row_off_y + i + 1] = yuyv[idx + 2];   // Y1
            u[uv_idx]            = yuyv[idx + 1];   // U for the pair
            v[uv_idx]            = yuyv[idx + 3];   // V for the pair
        }
    }
}

int main(void)
{
    FILE *in = fopen(INPUT_FILE, "rb");
    if (!in) { perror("open input"); return 1; }
    FILE *out = fopen(OUTPUT_FILE, "wb");
    if (!out) { perror("open output"); fclose(in); return 1; }

    // --- x265 setup ---
    x265_param *param = x265_param_alloc();
    if (!param) { fprintf(stderr, "x265_param_alloc failed\n"); fclose(in); fclose(out); return 1; }

    x265_param_default(param);

    // IMPORTANT: Apply preset BEFORE setting width/height/fps/etc.
    x265_param_default_preset(param, "medium", NULL);
    // Don't apply "main422-8" (not a valid profile string). I422 is selected via internalCsp.

    param->sourceWidth   = WIDTH;
    param->sourceHeight  = HEIGHT;
    param->fpsNum        = FPS;
    param->fpsDenom      = 1;
    param->internalCsp   = X265_CSP_I422;   // 8-bit 4:2:2 planar input
    param->bRepeatHeaders= 1;               // write VPS/SPS/PPS periodically
    param->bAnnexB       = 1;               // raw Annex-B bitstream output

    // Simple quality/latency settings
    param->rc.rateControlMode = X265_RC_CRF;
    param->rc.rfConstant      = 20;         // lower = better quality (larger file)
    param->keyframeMax        = FPS * 2;    // ~2s GOP

    x265_encoder *encoder = x265_encoder_open(param);
    if (!encoder) {
        fprintf(stderr, "x265_encoder_open failed\n");
        x265_param_free(param);
        fclose(in); fclose(out);
        return 1;
    }

    x265_picture *pic_in = x265_picture_alloc();
    if (!pic_in) {
        fprintf(stderr, "x265_picture_alloc failed\n");
        x265_encoder_close(encoder);
        x265_param_free(param);
        fclose(in); fclose(out);
        return 1;
    }
    x265_picture_init(param, pic_in);

    const size_t y_size   = (size_t)WIDTH * HEIGHT;
    const size_t uv_size  = (size_t)(WIDTH / 2) * HEIGHT;
    const size_t frame_sz = (size_t)WIDTH * HEIGHT * 2; // YUYV packed: 2 bytes/px

    uint8_t *planeY  = (uint8_t*)malloc(y_size);
    uint8_t *planeU  = (uint8_t*)malloc(uv_size);
    uint8_t *planeV  = (uint8_t*)malloc(uv_size);
    uint8_t *yuyvBuf = (uint8_t*)malloc(frame_sz);
    if (!planeY || !planeU || !planeV || !yuyvBuf) {
        fprintf(stderr, "malloc failed\n");
        free(planeY); free(planeU); free(planeV); free(yuyvBuf);
        x265_picture_free(pic_in);
        x265_encoder_close(encoder);
        x265_param_free(param);
        fclose(in); fclose(out);
        return 1;
    }

    pic_in->planes[0] = planeY;
    pic_in->planes[1] = planeU;
    pic_in->planes[2] = planeV;
    pic_in->stride[0] = WIDTH;
    pic_in->stride[1] = WIDTH / 2;
    pic_in->stride[2] = WIDTH / 2;

    int64_t pts = 0;
    for (;;) {
        size_t rd = fread(yuyvBuf, 1, frame_sz, in);
        if (rd == 0) break;                 // EOF
        if (rd != frame_sz) {               // partial frame (size mismatch)
            fprintf(stderr, "warning: partial frame read (%zu/%zu), stopping\n", rd, frame_sz);
            break;
        }

        yuyv_to_i422(yuyvBuf, planeY, planeU, planeV, WIDTH, HEIGHT);
        pic_in->pts = pts++;                // one tick per frame at fpsNum/fpsDenom

        x265_nal *nals = NULL;
        uint32_t nal_count = 0;
        int bytes = x265_encoder_encode(encoder, &nals, &nal_count, pic_in, NULL);
        if (bytes < 0) { fprintf(stderr, "encode error\n"); break; }

        for (uint32_t i = 0; i < nal_count; i++)
            fwrite(nals[i].payload, 1, nals[i].sizeBytes, out);
    }

    // Flush delayed frames (B-frames, etc.)
    for (;;) {
        x265_nal *nals = NULL;
        uint32_t nal_count = 0;
        int bytes = x265_encoder_encode(encoder, &nals, &nal_count, NULL, NULL);
        if (bytes <= 0) break;
        for (uint32_t i = 0; i < nal_count; i++)
            fwrite(nals[i].payload, 1, nals[i].sizeBytes, out);
    }

    free(yuyvBuf);
    free(planeY); free(planeU); free(planeV);
    x265_picture_free(pic_in);
    x265_encoder_close(encoder);
    x265_param_free(param);
    fclose(in); fclose(out);

    printf("OK: encoded %s from %s at %dx%d @ %dfps (I422)\n",
           OUTPUT_FILE, INPUT_FILE, WIDTH, HEIGHT, FPS);
    return 0;
}

