#include <stdio.h>

int divide(int a, int b, int *result)
{
    if (b == 0)
        return -1; 
    *result = a / b;
    return 0; 
}

int main()
{
    int res;
    if (divide(10, 0, &res) == -1)
        printf("Error: Division by zero\n");
    else
        printf("Result: %d\n", res);
    return 0;
}

