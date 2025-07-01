#include <stdio.h>

int main() {
    int arr[6] = {30, 10, 50, 20, 60, 40};
    int n = 6;

    // Bubble sort logic
    for (int i = 0; i < n - 1; i++) {

        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap adjacent elements
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        // If no two elements were swapped, break early
        if (swapped == 0)
            break;
    }

    // Print sorted array
    printf("Sorted array:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}

