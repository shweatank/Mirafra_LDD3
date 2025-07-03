#include <stdio.h>

unsigned int swap_bytes_32bit(unsigned int x) {
    return ((x & 0xFF000000) >> 24) |
           ((x & 0x00FF0000) >> 8)  |
           ((x & 0x0000FF00) << 8)  |
           ((x & 0x000000FF) << 24);
}

int main() {
    unsigned int num = 0x12345678;
    unsigned int result = swap_bytes_32bit(num);

    printf("Original: 0x%X\n", num);
    printf("Swapped : 0x%X\n", result);

    return 0;
}

