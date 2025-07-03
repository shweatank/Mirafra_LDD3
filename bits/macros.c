#include <stdio.h>

#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned int

#define SWAP_NIBBLES(x)  ( ((x & 0x0F) << 4) | ((x & 0xF0) >> 4) )
#define SWAP_BYTES_16(x) ( ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8) )
#define SWAP_BYTES_32(x) ( ((x & 0x000000FF) << 24) | \
                           ((x & 0x0000FF00) << 8)  | \
                           ((x & 0x00FF0000) >> 8)  | \
                           ((x & 0xFF000000) >> 24) )

#define SWAP_BITS(x) ( ((x & 0x01) << 7) | \
                       ((x & 0x02) << 5) | \
                       ((x & 0x04) << 3) | \
                       ((x & 0x08) << 1) | \
                       ((x & 0x10) >> 1) | \
                       ((x & 0x20) >> 3) | \
                       ((x & 0x40) >> 5) | \
                       ((x & 0x80) >> 7) )

int main() {
    u8 val8 = 0xAB;
    u16 val16 = 0x1234;
    u32 val32 = 0x12345678;

    printf("Swap nibbles  0x%X -> 0x%X\n", val8, SWAP_NIBBLES(val8));
    printf("Swap bits     0x%X -> 0x%X\n", val8, SWAP_BITS(val8));
    printf("Swap bytes 16 0x%X -> 0x%X\n", val16, SWAP_BYTES_16(val16));
    printf("Swap bytes 32 0x%X -> 0x%X\n", val32, SWAP_BYTES_32(val32));

    return 0;
}

