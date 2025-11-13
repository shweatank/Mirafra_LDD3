#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int *)malloc(sizeof(int) * 5);
    ptr = (int *)malloc(sizeof(int) * 10); // The old 5-element block is lost

    free(ptr); // Frees only the new block
    return 0;
}

