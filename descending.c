#include <stdio.h>

int main() {
    int arr[6] = {30, 10, 50, 20, 60, 40};
    int n = 6;

    // Bubble sort in descending order
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] < arr[j + 1]) {  // change comparison for descending
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }

    // Print the sorted array
    printf("Sorted array in descending order:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}

