/*** array of function pointer ***/

#include <stdio.h>

int add(int a,int b)
{
    return a + b;
}
int sub(int a,int b)
{
    return a - b;
}
int mul(int a,int b)
{
    return a * b;
}
int main()
{
    int (*fptr[])(int,int) = {add,sub,mul};
    printf("add: %d\n", fptr[0](10, 5));  // 15
    printf("sub: %d\n", fptr[1](10, 5));  // 5
    printf("mul: %d\n", fptr[2](10, 5));  // 50
}
