//left rotate
#include <stdio.h>
#include <stdint.h>

unsigned int leftRotate(uint32_t x, unsigned int n) {
    return (x << n) | (x >> (32 - n));
}

int main() {
   unsigned int num = 0xA1B2C3D4; 
    unsigned int n = 5;        
    unsigned result = leftRotate(num, n);
    printf("After left rotation: 0x%X\n", result);
    return 0;
}

