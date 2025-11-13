#include <stdio.h>

int isPowerOfTwo(unsigned int n) {
    if (n == 0)
        return 0;
    return (n & (n - 1)) == 0;
}

int main() {
    unsigned int test[] = {0, 1, 2, 3, 4, 5, 8, 16, 18, 64};
    for (int i = 0; i < 10; i++) {
        printf("%2u is power of 2? %s\n", test[i], isPowerOfTwo(test[i]) ? "Yes" : "No");
    }
    return 0;
}

