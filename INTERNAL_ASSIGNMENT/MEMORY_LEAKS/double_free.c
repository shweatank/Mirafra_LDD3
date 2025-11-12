#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int*)malloc(sizeof(int));
    *ptr = 42;

    free(ptr);  // first free
    free(ptr);  // BUG: double free

    return 0;
}

