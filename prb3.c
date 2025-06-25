#include <stdio.h>
#include <string.h>

void sortStrings(char *arr[], int n) {
    for (int i = 0; i < n -1; i++) {
        for (int j = i +1; j < n; j++) {
            if (strcmp(arr[i], arr[j]) > 0) {
                char *temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int main() {
    char *arr[] = {"banana", "apple", "orange", "grape"};
    int n = sizeof(arr) / sizeof(arr[0]);

    sortStrings(arr, n);

    printf("Sorted strings:\n");
    for (int i = 0; i < n; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");

   
}

