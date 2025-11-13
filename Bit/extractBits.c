#include <stdio.h>

unsigned int extractBits(unsigned int n, int start, int len) {
    // Step 1: Shift right to align target bits to LSB
    unsigned int shifted = n >> start;

    // Step 2: Mask the lowest 'len' bits
    unsigned int mask = (1U << len) - 1;

    return shifted & mask;
}

void printBits(unsigned int n) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
}

int main() {
    unsigned int n = 0b11010110;

    printf("Original: ");
    printBits(n);

    int start = 1, len = 3;
    unsigned int result = extractBits(n, start, len);

    printf("Extracted %d bits from position %d: %u (binary ", len, start, result);
    printBits(result);
    printf(")\n");

    return 0;
}

