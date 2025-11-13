
#include <stdio.h>

unsigned int swapBits(unsigned int n, int pos1, int pos2) {
    unsigned int bit1 = (n >> (pos1 - 1)) & 1;
    unsigned int bit2 = (n >> (pos2 - 1)) & 1;

    if (bit1 == bit2)
        return n;

    // Toggle both bits using XOR
    n ^= (1U << (pos1 - 1)) | (1U << (pos2 - 1));
    return n;
}

void printBits(unsigned int n) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
        if (i % 8 == 0) printf(" ");
    }
    printf("\n");
}

int main() {
    unsigned int n = 0b01010000;
    int pos1 = 5, pos2 = 1;

    printf("Original:        ");
    printBits(n);

    unsigned int swapped = swapBits(n, pos1, pos2);

    printf("After Swap %d <-> %d:\n", pos1, pos2);
    printBits(swapped);
    printf("Decimal Output:  %u\n", swapped);

    return 0;
}

