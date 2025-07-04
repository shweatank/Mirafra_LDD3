#include <stdio.h>

int main() {
    unsigned int num;
    int i;

    printf("Enter an integer: ");
    scanf("%u", &num);

    printf("\nOriginal number in binary:\n");
    for (i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }

    unsigned int first_nibble = (num >> 28) & 0xF;
    unsigned int last_nibble = num & 0xF;

    num = num & 0x0FFFFFF0;
    num = num & ~(0xF << 28);

    num = num | (last_nibble << 28);
    num = num | first_nibble;

    printf("\n\nAfter swapping first and last nibbles:\n");
    for (i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }

    printf("\n\nResult in decimal: %u\n", num);

    return 0;
}

