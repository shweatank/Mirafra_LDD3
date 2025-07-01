//xor of first n numbers
#include <stdio.h>

int xor_1_to_n(int n) {
    if (n % 4 == 0)
        return n;
    else if (n % 4 == 1)
        return 1;
    else if (n % 4 == 2)
        return n + 1;
    else
        return 0;
}

int main() {
    int n;

    printf("Enter N: ");
    scanf("%d", &n);

    printf("XOR from 1 to %d is: %d\n", n, xor_1_to_n(n));

    return 0;
}

