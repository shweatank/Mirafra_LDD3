#include <stdio.h>
unsigned int reverse_hex(unsigned int num) {
    unsigned int n1 = (num & 0x000F) << 12;  // Last nibble to first
    unsigned int n2 = (num & 0x00F0) << 4;   // 2nd nibble to 2nd
    unsigned int n3 = (num & 0x0F00) >> 4;   // 3rd nibble to 3rd
    unsigned int n4 = (num & 0xF000) >> 12;  // First nibble to last
    return (n1 | n2 | n3 | n4);
}
int main() {
    unsigned int num = 0x1234;
    unsigned int reversed = reverse_hex(num);
    printf("Original: %#X\n", num);
    printf("Reversed: %#X\n", reversed);
    return 0;
}

