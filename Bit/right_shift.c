#include <stdio.h>

int main() {
    unsigned int u = 0b10000000;  // 128
    int s = -128;

    printf("Unsigned Right Shift: %u >> 1 = %u\n", u, u >> 1);
    printf("Signed Right Shift:   %d >> 1 = %d\n", s, s >> 1);

    return 0;
}

