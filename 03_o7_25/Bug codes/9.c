#include <stdio.h>

int main() {
    int *p;
    *p = 10;  // Bug: using uninitialized pointer
    printf("%d\n", *p);
    return 0;
}

//answer
//int *p=10
// or
//x=10;
// int *p=&x;
//*p=10;
