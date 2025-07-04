#include <stdio.h>

int main() {
    int x = 5;
    if(x = 10)  // Bug: assignment instead of comparison (==)
        printf("x is 10\n");
    return 0;
}
//answer
//at if condition
//if(x==10)
