#include<stdio.h>

unsigned int swap_nibbles_32bit(unsigned int x) {
    unsigned int result = 0;

    for (int i = 0; i < 4; i++) {
        unsigned char byte = (x >> (i * 8)) & 0xFF;
        unsigned char swapped = ((byte & 0x0F) << 4) | ((byte & 0xF0) >> 4);
        result |= (swapped << (i * 8));
    }

    return result;
}

int main() {
    unsigned int num = 0x12345678;
    unsigned int result = swap_nibbles_32bit(num);

    printf("Original: 0x%X\n", num);
    printf("Swapped : 0x%X\n", result);

    return 0;
}

