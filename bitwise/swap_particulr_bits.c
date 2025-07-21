#include <stdio.h>
unsigned int swapBits(unsigned int number, unsigned int i, unsigned int j) {
    if (((number >> i) & 1) != ((number >> j) & 1)) {
        number ^= (1 << i);
        number ^= (1 << j);
    }
    return number;
}
int main() {
    unsigned int num = 0b10010100;  // 180
    unsigned int i = 2;
    unsigned int j = 5;

    printf("Original : %#X (%u)\n", num, num);
    unsigned int swapped = swapBits(num, i, j);
    printf("Swapped  : %#X (%u)\n", swapped, swapped);
    printf("0b");
    for(int i=7;i>=0;i--)
    {
        printf("%d",((swapped>>i)&1));
    }
}
// o/p  : 10100110
