#include <stdio.h>

// Recursive function to count set bits
int countSetBits(unsigned int n) {
    if (n == 0)
        return 0;
    return (n & 1) + countSetBits(n >> 1);
}

int main() {
    unsigned int n;
    printf("Enter a number: ");
    scanf("%u", &n);

    int count = countSetBits(n);

    printf("Number of set bits = %d\n", count);
    return 0;
}

