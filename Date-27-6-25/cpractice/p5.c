#include <stdio.h>

int main() {
    int start = 10, goal = 7;
    int xor = start ^ goal;
    int count = 0;
    while (xor) {
        xor &= (xor - 1);
        count++;
    }
    printf("%d\n", count);
    return 0;
}

