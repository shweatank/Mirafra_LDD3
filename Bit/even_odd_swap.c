//Swap even and odd bits of an unsigned integer number

#include <stdio.h>
#include <stdint.h>

uint32_t swapEvenOddBits(uint32_t n) {
    uint32_t evenBits = n & 0xAAAAAAAA; // mask even positions
    uint32_t oddBits  = n & 0x55555555; // mask odd positions

    evenBits >>= 1; // move even bits to odd positions
    oddBits  <<= 1; // move odd bits to even positions

    return (evenBits | oddBits); // combine
}

int main() {
    uint32_t num = 23; // binary: 0001 0111
    uint32_t result = swapEvenOddBits(num);

    printf("Original number : %u (0x%X)\n", num, num);
    printf("After swapping  : %u (0x%X)\n", result, result);

    return 0;
}

/*OUTPUT: 
Original number : 23 (0x17)
After swapping  : 43 (0x2B)*/
