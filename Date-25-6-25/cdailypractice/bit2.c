#include <stdio.h>

int main() {
    int x = 6;
    int m = 1;
    while (x & m) {
        x = x ^ m;
        m <<= 1;
    }
    x = x ^ m;
    printf("x + 1 = %d\n", x);
    return 0;
}

