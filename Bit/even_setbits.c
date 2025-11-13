#include <stdio.h>

int hasEvenSetBits(unsigned int n) {
    int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count % 2 == 0;
}

void printBits(unsigned int n) {
    for (int i = 31; i >= 0; i--)
        printf("%d", (n >> i) & 1);
    printf("\n");
}

int main() {
    unsigned int values[] = {0, 1, 3, 7, 15, 16, 31, 32, 63};
    for (int i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        printf("n = %2u → ", values[i]);
        printBits(values[i]);
        printf("Even set bits? → %s\n\n", hasEvenSetBits(values[i]) ? "YES" : "NO");
    }
    return 0;
}

