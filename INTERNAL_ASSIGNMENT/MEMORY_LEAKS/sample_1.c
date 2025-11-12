#include <stdio.h>
#include <stdlib.h>

int main() {
    int *arr = (int*)malloc(5 * sizeof(int)); // allocated memory
    arr[0] = 10;
    arr[1] = 20;

    printf("arr[0] = %d\n", arr[0]);
    printf("arr[1] = %d\n", arr[1]);

    // BUG: forgot to free allocated memory
    return 0;
}

