#include <stdio.h>
#include <stdlib.h>

int main() {
    for (int i = 0; i < 5; i++) {
        int *ptr = malloc(sizeof(int) * 100);
        // Using ptr...
        // Forgot to free(ptr);
    }
    return 0;
}

