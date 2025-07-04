#include <stdlib.h>
#include <stdio.h>

int main() {
    int *p = malloc(sizeof(int));
    *p = 42;
    free(p);
    printf("%d\n", *p);  // Bug: using memory after it was freed
    return 0;
}
// answer
// dereference *p before free the pointer
