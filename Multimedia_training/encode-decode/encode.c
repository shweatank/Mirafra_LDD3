// Compile with:
// gcc encode_h265.c -o encode_h265 -lx265

#include <x265.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    x265_param *param = x265_param_alloc();
    if (!param) {
        fprintf(stderr, "Failed to allocate x265 params\n");
        return -1;
    }

    // Default preset: medium, tune: none
    x265_param_default(param);
    param->sourceWidth = 1920;  // Frame width
    param->sourceHeight = 1080; // Frame height
    param->fpsNum = 30;
    param->fpsDenom = 1;
    param->internalCsp = X265_CSP_I420; // YUV420 format

    // Create encoder
    x265_encoder *encoder = x265_encoder_open(param);
    if (!encoder) {
        fprintf(stderr, "Failed to open x265 encoder\n");
        x265_param_free(param);
        return -1;
    }

    x265_picture *pic_in = x265_picture_alloc();
    x265_picture *pic_out = x265_picture_alloc();
    x265_picture_init(param, pic_in);

    // Allocate raw YUV buffer (I420)
    size_t y_size = param->sourceWidth * param->sourceHeight;
    size_t uv_size = y_size / 4;
    pic_in->planes[0] = malloc(y_size);   // Y plane
    pic_in->planes[1] = malloc(uv_size);  // U plane
    pic_in->planes[2] = malloc(uv_size);  // V plane
    pic_in->stride[0] = param->sourceWidth;
    pic_in->stride[1] = param->sourceWidth / 2;
    pic_in->stride[2] = param->sourceWidth / 2;

    FILE *out = fopen("output.h265", "wb");
    if (!out) {
        perror("fopen");
        return -1;
    }

    // For demo, we encode just black frames
    memset(pic_in->planes[0], 0, y_size);
    memset(pic_in->planes[1], 128, uv_size);
    memset(pic_in->planes[2], 128, uv_size);

    for (int i = 0; i < 60; i++) { // Encode 2 seconds @ 30fps
        x265_nal *nals;
        uint32_t nal_count;
        int frame_size = x265_encoder_encode(encoder, &nals, &nal_count, pic_in, pic_out);
        if (frame_size > 0) {
            for (uint32_t j = 0; j < nal_count; j++)
                fwrite(nals[j].payload, 1, nals[j].sizeBytes, out);
        }
    }

    // Flush encoder
    x265_nal *nals;
    uint32_t nal_count;
    while (x265_encoder_encode(encoder, &nals, &nal_count, NULL, pic_out)) {
        for (uint32_t j = 0; j < nal_count; j++)
            fwrite(nals[j].payload, 1, nals[j].sizeBytes, out);
    }

    fclose(out);

    // Free resources
    free(pic_in->planes[0]);
    free(pic_in->planes[1]);
    free(pic_in->planes[2]);
    x265_picture_free(pic_in);
    x265_picture_free(pic_out);
    x265_encoder_close(encoder);
    x265_param_free(param);

    printf("Encoding done: output.h265\n");
    return 0;
}
