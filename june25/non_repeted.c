#include <stdio.h>
#define MAX 1000  
void find_non_repeating(int *arr, int n)
{
    int freq[MAX] = {0}; 
    for (int i = 0; i < n; i++)
    {
        freq[arr[i]]++;
    }
    printf("Non-repeating elements:\n");
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (freq[arr[i]]==1) {
            printf("%d ", arr[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("None");
    }
    printf("\n");
}

int main() 
{
    int n;
    printf("Enter number of elements:\n");
    scanf("%d", &n);
    int arr[n];
    printf("Enter elements:\n");
    for (int i=0;i<n;i++)
    {
        scanf("%d", &arr[i]);
    }
    find_non_repeating(arr, n);
    return 0;
}

