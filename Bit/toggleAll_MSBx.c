//Toggle all the bits of a number except MSB
#include <stdio.h>
#include <stdint.h>

uint32_t toggleExceptMSB(uint32_t n) {
    // Keep MSB as is, toggle lower 31 bits
    return (n & 0x80000000) | (~n & 0x7FFFFFFF);
}

int main() {
    uint32_t num = 0x8000000F;  // 1000....0001111
    uint32_t result = toggleExceptMSB(num);

    printf("Original : 0x%X\n", num);
    printf("Toggled  : 0x%X\n", result);

    return 0;
}

