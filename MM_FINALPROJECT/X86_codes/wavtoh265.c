// encode_yuyv_i422.c - Encode raw YUYV422 video to H.265 (I422) using x265
// Build:   gcc encode_yuyv_i422.c -o encode_yuyv_i422 -lx265
// Input:   raw YUYV422 frames captured at 1280x720 @ 10fps
// Output:  raw Annex-B H.265 bitstream (output.h265)

#include <x265.h>     // x265 encoder API
#include <stdio.h>    // Standard I/O (fopen, fwrite, printf)
#include <stdlib.h>   // malloc, free, exit
#include <stdint.h>   // uint8_t, int32_t, etc.

// ---------- Video configuration ----------
#define WIDTH       1280      // Video width
#define HEIGHT      720       // Video height
#define FPS         10        // Frames per second
#define INPUT_FILE  "video.yuv"    // Raw input frames (YUYV422)
#define OUTPUT_FILE "output.h265"  // H.265 bitstream output

// --------- Convert YUYV422 (packed) -> I422 (planar) ---------
static void yuyv_to_i422(const uint8_t *yuyv, uint8_t *y, uint8_t *u, uint8_t *v,
                         int width, int height)
{
    for (int j = 0; j < height; j++) {
        const int row_off_y  = j * width;         // Offset for Y plane row
        const int row_off_uv = j * (width / 2);   // Offset for U/V planes row (4:2:2)
        for (int i = 0; i < width; i += 2) {
            int idx    = (row_off_y + i) * 2;     // Index in YUYV packed buffer
            int uv_idx = row_off_uv + (i >> 1);   // Index in planar U/V buffer

            // YUYV byte order: Y0 U Y1 V
            y[row_off_y + i]     = yuyv[idx];       // Extract Y0
            y[row_off_y + i + 1] = yuyv[idx + 2];   // Extract Y1
            u[uv_idx]            = yuyv[idx + 1];   // Extract U for pair
            v[uv_idx]            = yuyv[idx + 3];   // Extract V for pair
        }
    }
}

int main(void)
{
    // Open input raw YUYV file
    FILE *in = fopen(INPUT_FILE, "rb");
    if (!in) { perror("open input"); return 1; }

    // Open output H.265 bitstream file
    FILE *out = fopen(OUTPUT_FILE, "wb");
    if (!out) { perror("open output"); fclose(in); return 1; }

    // -------------------- x265 encoder setup --------------------
    x265_param *param = x265_param_alloc();   // Allocate parameter struct
    if (!param) { fprintf(stderr, "x265_param_alloc failed\n"); fclose(in); fclose(out); return 1; }

    x265_param_default(param);                // Fill with default parameters

    x265_param_default_preset(param, "medium", NULL); // Apply preset for speed/quality tradeoff

    // Set video resolution and frame rate
    param->sourceWidth   = WIDTH;  
    param->sourceHeight  = HEIGHT;
    param->fpsNum        = FPS;
    param->fpsDenom      = 1;

    param->internalCsp   = X265_CSP_I422;     // Input is I422 planar (8-bit)
    param->bRepeatHeaders= 1;                 // Write VPS/SPS/PPS periodically
    param->bAnnexB       = 1;                 // Raw Annex-B output (start codes)

    // Rate control: constant quality
    param->rc.rateControlMode = X265_RC_CRF;
    param->rc.rfConstant      = 20;           // Quality factor (lower = better)
    param->keyframeMax        = FPS * 2;      // Maximum GOP length ~2 seconds

    // Open encoder instance
    x265_encoder *encoder = x265_encoder_open(param);
    if (!encoder) {
        fprintf(stderr, "x265_encoder_open failed\n");
        x265_param_free(param);
        fclose(in); fclose(out);
        return 1;
    }

    // Allocate picture structure
    x265_picture *pic_in = x265_picture_alloc();
    if (!pic_in) {
        fprintf(stderr, "x265_picture_alloc failed\n");
        x265_encoder_close(encoder);
        x265_param_free(param);
        fclose(in); fclose(out);
        return 1;
    }
    x265_picture_init(param, pic_in); // Initialize picture to match encoder parameters

    // Calculate plane sizes
    const size_t y_size   = (size_t)WIDTH * HEIGHT;         // Y plane
    const size_t uv_size  = (size_t)(WIDTH / 2) * HEIGHT;   // U and V planes
    const size_t frame_sz = (size_t)WIDTH * HEIGHT * 2;     // YUYV packed: 2 bytes per pixel

    // Allocate memory for Y, U, V planes and input buffer
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

    // Assign planes to picture struct
    pic_in->planes[0] = planeY;
    pic_in->planes[1] = planeU;
    pic_in->planes[2] = planeV;
    pic_in->stride[0] = WIDTH;        // Y stride = width
    pic_in->stride[1] = WIDTH / 2;    // U stride = width/2 (4:2:2)
    pic_in->stride[2] = WIDTH / 2;    // V stride = width/2

    int64_t pts = 0;   // Presentation timestamp counter

    // -------------------- Main encoding loop --------------------
    for (;;) {
        size_t rd = fread(yuyvBuf, 1, frame_sz, in);  // Read one raw frame
        if (rd == 0) break;                            // EOF
        if (rd != frame_sz) {                           // Partial frame read
            fprintf(stderr, "warning: partial frame read (%zu/%zu), stopping\n", rd, frame_sz);
            break;
        }

        yuyv_to_i422(yuyvBuf, planeY, planeU, planeV, WIDTH, HEIGHT); // Convert packed YUYV -> planar I422
        pic_in->pts = pts++;                             // Assign frame timestamp

        x265_nal *nals = NULL;                          // NAL units returned by encoder
        uint32_t nal_count = 0;
        int bytes = x265_encoder_encode(encoder, &nals, &nal_count, pic_in, NULL); // Encode frame
        if (bytes < 0) { fprintf(stderr, "encode error\n"); break; }

        // Write all encoded NAL units to output file
        for (uint32_t i = 0; i < nal_count; i++)
            fwrite(nals[i].payload, 1, nals[i].sizeBytes, out);
    }

    // Flush delayed frames (B-frames, lookahead)
    for (;;) {
        x265_nal *nals = NULL;
        uint32_t nal_count = 0;
        int bytes = x265_encoder_encode(encoder, &nals, &nal_count, NULL, NULL);
        if (bytes <= 0) break;
        for (uint32_t i = 0; i < nal_count; i++)
            fwrite(nals[i].payload, 1, nals[i].sizeBytes, out);
    }

    // -------------------- Cleanup --------------------
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

