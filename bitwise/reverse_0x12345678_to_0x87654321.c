#include <stdio.h>
unsigned int reverse_hex_32bit(unsigned int num) {
    return  ((num & 0x0000000F) << 28) |
            ((num & 0x000000F0) << 20) |
            ((num & 0x00000F00) << 12) |
            ((num & 0x0000F000) << 4)  |
            ((num & 0x000F0000) >> 4)  |
            ((num & 0x00F00000) >> 12) |
            ((num & 0x0F000000) >> 20) |
            ((num & 0xF0000000) >> 28);
}
int main() {
    unsigned int num = 0x12345678;
    unsigned int reversed = reverse_hex_32bit(num);
    printf("Original: 0x%X\n", num);
    printf("Reversed: 0x%X\n", reversed);
    return 0;
}

