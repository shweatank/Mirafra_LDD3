#include <stdio.h>

int main() {
    int arr[10] = {5, 10, 5, 20, 10, 30, 20, 40, 30, 50};
    int unique[10];
    int size = 0;

    // Remove duplicates
    for (int i = 0; i < 10; i++) {
        int isDuplicate = 0;
        for (int j = 0; j < size; j++) {
            if (arr[i] == unique[j]) {
                isDuplicate = 1;
                break;
            }
        }
        if (!isDuplicate)
            unique[size++] = arr[i];
    }

    // Print the unique elements
    printf("Array after removing duplicates:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", unique[i]);
    }
    printf("\n");

    return 0;
}

