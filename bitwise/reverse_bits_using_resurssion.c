#include <stdio.h>

void print_binary(unsigned int n) {
    for (int i = 31; i >= 0; i--)
        printf("%d", (n >> i) & 1);
}

// Recursive function to reverse bits
unsigned int reverse_bits(unsigned int n, int bit_pos, unsigned int result) {
    if (bit_pos == 0)
        return result;

    result <<= 1;              // Make space for next bit
    result |= (n & 1);         // Copy LSB of n
    return reverse_bits(n >> 1, bit_pos - 1, result);
}

int main() {
    unsigned int n;
    scanf("%u", &n);

    printf("Original binary : ");
    print_binary(n);
    printf("\tDecimal: %u\n", n);

    unsigned int reversed = reverse_bits(n, 32, 0);

    printf("Reversed binary : ");
    print_binary(reversed);
    printf("\tDecimal: %u\n", reversed);

    return 0;
}

