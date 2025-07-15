#include <stdio.h>
#include <stdint.h>

uint32_t updateBits(uint32_t n) {
    if (n & (1 << 5)) {
        n &= ~((1 << 12) | (1 << 13)); // Clear 12th & 13th bits

        uint32_t bit27 = (n >> 27) & 1;
        uint32_t bit29 = (n >> 29) & 1;

        n |= (bit27 << 12);
        n |= (bit29 << 13);
    }
    return n;
}

int main() {
    uint32_t num;
    printf("Enter number: ");
    scanf("%u", &num);
    uint32_t result = updateBits(num);
    printf("Updated number: %u (0x%X)\n", result, result);
    return 0;
}

