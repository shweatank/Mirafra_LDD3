#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void risky_function() {
    printf("In risky function\n");
    longjmp(jump_buffer, 10);  // simulate exception
}

int main() {
    if (setjmp(jump_buffer) == 0) {
        risky_function();
        printf("This won't print\n");
    } else {
        printf("Exception caught using longjmp!\n");
    }
    return 0;
}

