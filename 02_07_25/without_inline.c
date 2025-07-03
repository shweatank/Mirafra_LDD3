#include <stdio.h>

int square(int x) {  // normal function
    return x * x;
}

int main() {
    int a = 5;
    int result = square(a);  // function call
    printf("Square: %d\n", result);
    return 0;
}

