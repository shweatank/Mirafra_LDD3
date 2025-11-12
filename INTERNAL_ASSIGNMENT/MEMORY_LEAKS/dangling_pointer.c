#include <stdio.h>
#include <stdlib.h>

int* get_pointer() {
    int local_var = 10;
    return &local_var; // BUG: returning address of local variable
}

int main() {
    int *ptr = get_pointer();
    printf("%d\n", *ptr); // Undefined behavior

    return 0;
}

