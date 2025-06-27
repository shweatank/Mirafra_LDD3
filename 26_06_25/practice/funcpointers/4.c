//call a vardiadic function
#include <stdio.h>
#include <stdarg.h>
typedef void (*print_fn)(const char *fmt, ...);
void my_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);  
    va_end(args);
}

int main() {
    print_fn printer = my_printf;
    printer("Hello %s, number = %d, float = %.2f\n", "World", 42, 3.14159);

    return 0;
}

