#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main() {
    printf("Sum = %d\n", add(3, 4));
    return 0;
}

/*gcc -fPIC -c mathlib.c -o mathlib.o   # Compile to Position Independent Code
gcc -shared -o libmath.so mathlib.o   # Create shared library
gcc mathlib.c -L. -lmath -o dyn_app   # Link dynamically*/

