#include <stdio.h>

unsigned int replaceBits(unsigned int N, unsigned int M, int i, int j) {
    unsigned int allOnes = ~0;  

    unsigned int left = allOnes << (j + 1);        
    unsigned int right = (1 << i) - 1;            
    unsigned int mask = left | right;              
    unsigned int n_cleared = N & mask;
    unsigned int m_shifted = M << i;
    return n_cleared | m_shifted;
}

int main() {
    unsigned int N = 0b10000000000; 
    unsigned int M = 0b10101;       
    int i = 2, j = 6;

    unsigned int result = replaceBits(N, M, i, j);
    printf("Result: %u (binary: 0b", result);
    for (int k = 10; k >= 0; k--) {
        printf("%d", (result >> k) & 1);
    }
    printf(")\n");
    return 0;
}

