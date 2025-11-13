//Extract Bits Between Two Positions                                                        
#include <stdio.h>
#include <stdint.h>

uint32_t extractBits(uint32_t num, int low, int high) {
    if (low > high || high >= 32) return 0;  // invalid input safety

    uint32_t mask = ((1u << (high - low + 1)) - 1) << low;
    return (num & mask) >> low;
}

int main() {
    uint32_t num = 0x1B5; // binary: 110110101
    int low = 2, high = 5;

    uint32_t result = extractBits(num, low, high);

    printf("Number : 0x%X\n", num);
    printf("Bits[%d:%d] = %u (0x%X)\n", high, low, result, result);

    return 0;
}

