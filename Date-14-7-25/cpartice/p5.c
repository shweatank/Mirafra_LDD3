#include <stdio.h>

#define MAX 1001

int main() {
    int n;
    printf("Enter array size: ");
    scanf("%d", &n);

    int arr[n], freq[MAX] = {0};
    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
        freq[arr[i]]++;
    }

    int index = 0;
    for (int i = 0; i < n; i++) {
        if (freq[arr[i]] == 1) {
            arr[index++] = arr[i];
        }
    }

    printf("Array after removing duplicates (updated in-place):\n");
    for (int i = 0; i < index; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}

