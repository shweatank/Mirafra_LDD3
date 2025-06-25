#include <stdio.h>

int BtoD(int arr[], int size) {
    int dec = 0;
    for (int i = 0; i < size; i++) {
        dec = (dec << 1) | arr[i];
        //dec = dec * 2 + arr[i];
    }
    return dec;
}

int main() {
    int arr[4] = {0, 1, 1, 0};
    int size = sizeof(arr) / sizeof(arr[0]);

    int result =BtoD(arr, size);
    printf("Decimal value: %d\n", result);

    return 0;
}

