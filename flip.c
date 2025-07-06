//Count number of bits to flip to convert A to B
#include <stdio.h>

int countSetBits(int n) {
    int count = 0;
    while (n) {
        n = n & (n - 1);  // Brian Kernighan's Algorithm
        count++;
    }
    return count;
}

// Function to count number of bits to flip
int countBitsToFlip(int a, int b) {
    int xor = a ^ b;
    return countSetBits(xor);
}

int main() {
    int a, b;

    printf("Enter two numbers: ");
    scanf("%d %d", &a, &b);

    int flips = countBitsToFlip(a, b);

    printf("Number of bits to flip to convert %d to %d: %d\n", a, b, flips);

    return 0;
}

