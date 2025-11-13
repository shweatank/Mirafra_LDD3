//Check 5th bit is set, if yes then clear 12th and 13th bits                           
// and update these with 27th and 29th bits

#include <stdio.h>
#include <stdint.h>

uint32_t updateBits(uint32_t n) {
    // Check if 5th bit is set
    if (n & (1 << 5)) {
        // Clear 12th and 13th bits
        n &= ~((1 << 12) | (1 << 13));

        // Get 27th and 29th bits
        uint32_t bit27 = (n >> 27) & 1;
        uint32_t bit29 = (n >> 29) & 1;

	// Update them into 12th and 13th
	n |= (bit27 << 12);
        n |= (bit29 << 13);
    }
    return n;
}

int main() {
    uint32_t num =0x2A0020;  // just a sample number with some bits set
    printf("Original: 0x%X\n", num);

    uint32_t result = updateBits(num);
    printf("Updated : 0x%X\n", result);

    return 0;
}

