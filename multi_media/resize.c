#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // for saving output

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <input_image> <output_image> <new_width> <new_height>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];
    int new_width = atoi(argv[3]);
    int new_height = atoi(argv[4]);

    int width, height, channels;
    unsigned char *input_data = stbi_load(input_file, &width, &height, &channels, 0);
    if (!input_data) {
        fprintf(stderr, "Error: Could not load image %s\n", input_file);
        return 1;
    }

    printf("Loaded %s (%dx%d, %d channels)\n", input_file, width, height, channels);

    unsigned char *output_data = malloc(new_width * new_height * channels);
    if (!output_data) {
        fprintf(stderr, "Error: Could not allocate memory\n");
        stbi_image_free(input_data);
        return 1;
    }

    if (!stbir_resize_uint8(input_data, width, height, 0,
                            output_data, new_width, new_height, 0, channels)) {
        fprintf(stderr, "Error: Resize failed\n");
        free(output_data);
        stbi_image_free(input_data);
        return 1;
    }

    // Save resized image as PNG
    if (!stbi_write_png(output_file, new_width, new_height, channels, output_data, new_width * channels)) {
        fprintf(stderr, "Error: Could not write image %s\n", output_file);
        free(output_data);
        stbi_image_free(input_data);
        return 1;
    }

    printf("Resized image saved to %s (%dx%d)\n", output_file, new_width, new_height);

    free(output_data);
    stbi_image_free(input_data);
    return 0;
}

