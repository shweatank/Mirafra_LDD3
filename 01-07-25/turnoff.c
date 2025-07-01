//turn off rightmost bit
#include <stdio.h>

int main() {
    int n;

    printf("Enter a number: ");
    scanf("%d", &n);

    int result = n & (n - 1);
    printf("After turning off rightmost set bit: %d\n", result);

    return 0;
}

