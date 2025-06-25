#include <stdio.h>

void sort(int *arr, int size, int (*compare)(int, int)) 
{
    for (int i = 0; i < size-1; i++)
    {
        for (int j = i+1; j < size; j++) 
	{
            if (compare(arr[i], arr[j])) 
	    {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int ascending(int a, int b) 
{ 
	return a > b; 
}
int descending(int a, int b) 
{ 
	return a < b; 
}

int main() {
    int arr[] = {5, 2, 9, 1, 3};
    int size = sizeof(arr)/sizeof(arr[0]);

    sort(arr, size, ascending);

    printf("Sorted Array: ");
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}

