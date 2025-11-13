#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int *)malloc(sizeof(int) * 5); // Allocates memory for 5 integers

    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    ptr[0] = 10;
    ptr[1] = 20;

    // Forgot to free(ptr);
    return 0;
}

