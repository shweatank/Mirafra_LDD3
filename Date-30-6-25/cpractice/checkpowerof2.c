#include <stdio.h>

int is_power_of_2(int n) 
{
    return (n != 0) && ((n & (n - 1)) == 0);
}

int main() 
{
    int n = 16;
    if (is_power_of_2(n))
        printf("Yes, power of 2\n");
    else
        printf("No\n");
    return 0;
}

