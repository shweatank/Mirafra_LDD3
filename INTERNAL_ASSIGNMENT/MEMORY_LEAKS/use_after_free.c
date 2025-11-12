#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int*)malloc(sizeof(int));
    *ptr = 100;

    free(ptr);      // free memory
    printf("%d\n", *ptr); // BUG: using memory after free

    return 0;
}

