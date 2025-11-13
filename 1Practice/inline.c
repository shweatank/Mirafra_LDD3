#include <stdio.h>

static int add(int a, int b)
{	
    return a + b;
}

int main() {
    int result = add(5, 3);
    printf("Result: %d\n", result);
    result = add(9, 1);
    printf("Result: %d\n", result);
    return 0;
}

