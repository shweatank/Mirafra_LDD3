#include <stdio.h>

inline int square(int x) {  // inline hint
    return x * x;
}

int main() {
    int a = 5;
    int result = square(a);  // might be inlined
    printf("Square: %d\n", result);
    return 0;
}

