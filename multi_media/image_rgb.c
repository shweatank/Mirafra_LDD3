#include <stdio.h>
#include <stdlib.h>

#define WIDTH 4
#define HEIGHT 3

int main() {
    FILE *fp = fopen("WhatsApp Image 2025-08-05 at 10.16.03 AM.jpeg", "rb");
    if (fp == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    unsigned char image[HEIGHT][WIDTH];

    // Read raw grayscale image data into the 2D array
    size_t read_size = fread(image, sizeof(unsigned char), WIDTH * HEIGHT, fp);
    fclose(fp);

    if (read_size != WIDTH * HEIGHT) {
        fprintf(stderr, "Unexpected file size. Expected %d bytes, got %zu\n", WIDTH * HEIGHT, read_size);
        return 1;
    }

    // Print the 2D array
    printf("Image Data (Grayscale):\n");
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            printf("%3d ", image[row][col]);  // Print as decimal
        }
        printf("\n");
    }

    return 0;
}
