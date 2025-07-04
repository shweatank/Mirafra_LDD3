#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int) * 10);
    ptr = malloc(sizeof(int) * 20);  // Bug: previous memory leaked
    return 0;
}
//answer
//free the first allocated memory or use another variable foe newly allocated memory
//int *ptr=malloc(sizeof(int)*10);
//free(ptr);
//ptr=malloc(sizeof(int)*20);
