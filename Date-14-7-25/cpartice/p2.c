#include <stdio.h>
#include <stdint.h>

uint32_t swapEvenOddBits(uint32_t n) {
    uint32_t even_bits = n & 0xAAAAAAAA;
    uint32_t odd_bits  = n & 0x55555555;
    even_bits >>= 1;
    odd_bits  <<= 1;
    return (even_bits | odd_bits);
}

int main() {
    uint32_t num;
    printf("Enter an unsigned integer: ");
    scanf("%u", &num);
    uint32_t swapped = swapEvenOddBits(num);
    printf("Original number      : %u (0x%X)\n", num, num);
    printf("After swapping bits  : %u (0x%X)\n", swapped, swapped);
    return 0;
}

