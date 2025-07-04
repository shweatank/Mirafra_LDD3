#include <stdio.h>

int main() {
    int *ptr;
    *ptr = 5;  //error
    printf("%d\n", *ptr);
    return 0;
}
//Answer:
//x=5;
//int *ptr=&x;
//*ptr=x;

