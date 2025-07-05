
#include <stdio.h>

unsigned int swapOddEvenBits(unsigned int number) {
    unsigned int result = 0;
    for (int i = 0; i < 4; i++) {
        unsigned int byte = (number >> (i * 8)) & 0xFF;
        unsigned int swappedByte = ((byte & 0x0F) << 4) | ((byte & 0xF0) >> 4);
        result |= (swappedByte << (i * 8));
    }
    return result;
}

/*void printBinary(unsigned int num) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
        if (i % 8 == 0) printf(" ");
    }
}*/

int main() {
    unsigned int num = 0x12345678;
    unsigned int swapped = swapOddEvenBits(num);
    printf("Original: %#X\n", num);
    //printBinary(num);
    //printf("\n");
    printf("Swapped : %#X\n", swapped);
    //printBinary(swapped);
    //printf("\n");
}

