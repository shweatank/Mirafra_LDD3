#include <stdio.h>
#include "file1.h"

int main() {
    // Get pointer to static add function
    add_func_ptr_t add_ptr = get_add_function();
    
    // Use it
    int result = add_ptr(10, 20);
    printf("10 + 20 = %d\n", result);

    return 0;
}

