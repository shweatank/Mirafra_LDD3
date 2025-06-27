//right rotate

#include <stdio.h>
#include <stdint.h>

unsigned int rightRotate(uint32_t x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

int main() {
    unsigned int num = 0xA1B2C3D4; // example 32-bit value
    unsigned int n = 5;        // rotate by 5 positions
    unsigned int result = rightRotate(num, n);
    printf("After right rotation: 0x%X\n", result);
    return 0;
}

