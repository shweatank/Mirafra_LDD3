// user_leak.c

#include <stdio.h>
#include <stdlib.h>

void cause_leak(int size)
{
    // 1. Allocation on the heap
    int *data = (int *)malloc(size * sizeof(int)); 

    if (data == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    // 2. Memory is used
    for (int i = 0; i < size; i++) {
        data[i] = i * 2;
    }
    
    // 3. The leak: The function exits without calling free(data)
    printf("Function completed. Memory address %p was allocated but not freed.\n", (void*)data);
   //  free(data); // <--- We intentionally omit this line
}

int main()
{
    printf("Starting user-space leak test...\n");
    cause_leak(100); // Leak 100 integers (400 bytes)
    printf("Program exiting.\n");
    return 0;
}
