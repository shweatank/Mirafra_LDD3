#include <stdio.h>
#include <stdint.h>

#define WIDTH 32
#define HEIGHT 32

// Example 32×32 bitmap
uint8_t hanuman_32x32[] = {
	#include "hanuman.h"
    // ... rest of bitmap ...
};

void previewBitmap(uint8_t *bitmap, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int byteIndex = (y * w + x) / 8;
            int bitIndex = 7 - (x % 8);
            int pixel = (bitmap[byteIndex] >> bitIndex) & 0x01;
            if (pixel)
                 printf(" "); // Black pixel
		 //printf("█");
	    else
                //printf(" "); // Black pixel
		printf("█"); // White pixel

        }
        printf("\n");
    }
}

int main() {
    previewBitmap(hanuman_32x32, WIDTH, HEIGHT);
    return 0;
}

