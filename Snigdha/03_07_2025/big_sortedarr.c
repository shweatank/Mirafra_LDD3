#include <stdio.h>

int main() {
    int s;
    printf("Enter size of array: ");
    scanf("%d", &s);
    int arr[s];

    printf("Enter array elements: ");
    for (int i = 0; i < s; i++)
        scanf("%d", &arr[i]);

    int c = 1, max = 1;
    int start_index = 0, temp_start = 0;

    for (int i = 1; i < s; i++) 
    {
        if (arr[i] >= arr[i - 1]) 
        {
            c++;
        } else 
        {
            if (c > max) 
            {
                max = c;
                start_index = temp_start;
            }
            c = 1;
            temp_start = i;
        }
    }

    // Final check in case longest subarray is at the end
    if (c > max) 
    {
        max = c;
        start_index = temp_start;
    }

    printf("Longest sorted subarray length: %d\n", max);
    printf("Longest sorted subarray: ");
    for (int i = 0; i < max; i++)
        printf("%d ", arr[start_index + i]);
    printf("\n");

    return 0;
}

