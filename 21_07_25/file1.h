#ifndef FILE1_H
#define FILE1_H

// Function pointer type for addition
typedef int (*add_func_ptr_t)(int, int);

// Declaration to get pointer to static add function
add_func_ptr_t get_add_function(void);

#endif

