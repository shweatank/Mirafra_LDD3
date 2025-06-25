#include <stdio.h>

int main() {
    int arr[] = {2, 3, 2, 4, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= arr[i];
    }
    printf("Single number is %d\n", result);
    return 0;
}

