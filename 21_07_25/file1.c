#include <stdio.h>

// Static function: only accessible in this file
static int add(int a, int b) {
    return a + b;
}

// Define function pointer type
typedef int (*add_func_ptr_t)(int, int);

// Expose function pointer to static function
add_func_ptr_t get_add_function() {
    return add;
}

