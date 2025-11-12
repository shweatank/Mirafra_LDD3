#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int*)malloc(10 * sizeof(int));
    if (!ptr) return 1;

    ptr = (int*)malloc(20 * sizeof(int)); // BUG: previous memory lost

    free(ptr); // only second allocation is freed

    return 0;
}

