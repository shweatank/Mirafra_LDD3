#include <stdio.h>
#include <stdint.h>

int countBits(int  n) 
{
    int count = 0;
    while (n) 
    {
        n &= (n - 1); 
        count++;
    }
    return count;
}

int hammingWeight(int n, int (*fp)(int))
{
    return fp(n);
}

int main() {
    uint32_t n = 0b00000000000000000000000011001011;

    int result = hammingWeight(n, countBits);
    printf("Number of 1 bits: %d\n", result);

    return 0;
}

