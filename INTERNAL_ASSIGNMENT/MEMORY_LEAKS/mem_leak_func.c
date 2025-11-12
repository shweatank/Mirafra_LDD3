#include <stdio.h>
#include <stdlib.h>

int* create_array(int size) {
    int *arr = (int*)malloc(size * sizeof(int));
    arr[0] = 1;
    arr[1] = 2;
    return arr; // caller must free
}

int main() {
    create_array(10); // BUG: return value ignored, memory leak

    return 0;
}

