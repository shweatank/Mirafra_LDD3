#include <stdio.h>
#include <stdlib.h>

// Memory Leak Example
void memory_leak_example() {
    printf("Memory Leak Example:\n");
    int *arr = malloc(10 * sizeof(int));
    arr[0] = 5;
    printf("Value: %d\n", arr[0]);
    // Forgot to free -> memory leak
}

// Segmentation Fault Example
void segfault_example() {
    printf("\nSegmentation Fault Example:\n");
    int *ptr = NULL;
    *ptr = 10; // Dereferencing NULL -> segfault
}

// Invalid Memory Access Example
void invalid_memory_access_example() {
    printf("\nInvalid Memory Access Example:\n");
    int *arr = malloc(5 * sizeof(int));
    arr[5] = 10; // Out of bounds write (indices 0-4)
    printf("Value: %d\n", arr[5]);
    free(arr);
}

int main() {
    // Uncomment the function you want to test one at a time
    memory_leak_example();
    // segfault_example();
    // invalid_memory_access_example();

    return 0;
}
/*
->compile 
and test with valgrind --leak-check=full ./test
*/
