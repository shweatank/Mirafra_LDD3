#include <stdio.h>
unsigned int swapBits(unsigned int number, unsigned int i, unsigned int j) {
    if (((number >> i) & 1) != ((number >> j) & 1)) {
        number ^= (1 << i);
        number ^= (1 << j);
    }
    return number;
}
int main() {
    unsigned int num = 0b10110100;  // 180
    unsigned int i = 2;
    unsigned int j = 5;

    printf("Original : %#X (%u)\n", num, num);
    unsigned int swapped = swapBits(num, i, j);
    printf("Swapped  : %#X (%u)\n", swapped, swapped);
}
// o/p  : 10100110
