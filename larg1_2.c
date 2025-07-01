#include <stdio.h>

int main() {
    int arr[5] = {10, 50, 30, 20, 40};
    int max1, max2;

    // Initialize max1 and max2
    if (arr[0] > arr[1]) {
        max1 = arr[0];
        max2 = arr[1];
    } else {
        max1 = arr[1];
        max2 = arr[0];
    }

    // Traverse the rest of the array
    for (int i = 2; i < 5; i++) {
        if (arr[i] > max1) {
            max2 = max1;
            max1 = arr[i];
        } else if (arr[i] > max2 && arr[i] != max1) {
            max2 = arr[i];
        }
    }

    printf("Largest element: %d\n", max1);
    printf("Second largest element: %d\n", max2);

    return 0;
}

