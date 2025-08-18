// Compile with:
// gcc decode_h265.c -o decode_h265 -lde265

#include <libde265/de265.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *in = fopen("input.h265", "rb");
    if (!in) {
        perror("fopen");
        return -1;
    }

    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    rewind(in);

    uint8_t *data = malloc(size);
    fread(data, 1, size, in);
    fclose(in);

    de265_decoder_context *ctx = de265_new_decoder();
    de265_error err;

    // Feed the input bitstream
    err = de265_push_data(ctx, data, size, 0);
    if (err != DE265_OK) {
        fprintf(stderr, "push_data error: %s\n", de265_get_error_text(err));
    }

    // Signal end-of-stream
    de265_push_end_of_stream(ctx);

    int more = 1;
    while (more) {
        err = de265_decode(ctx, &more);
        if (err != DE265_OK && err != DE265_ERROR_WAITING_FOR_INPUT_DATA) {
            fprintf(stderr, "decode error: %s\n", de265_get_error_text(err));
            break;
        }

        int img_ready = de265_get_number_of_ready_frames(ctx);
        while (img_ready > 0) {
            const de265_image *img = de265_get_next_picture(ctx);
            printf("Decoded frame: %d x %d\n", de265_get_image_width(img,0), de265_get_image_height(img,0));
            img_ready--;
        }
    }

    de265_free_decoder(ctx);
    free(data);

    printf("Decoding complete.\n");
    return 0;
}
