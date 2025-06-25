#include <stdio.h>
int main() 
{
    unsigned int n = 29; 
    int count = 0;
    while (n) 
    {
        n &= (n - 1);
        count++;
    }
    printf("Set bits in %u = %d\n", n, count);
    return 0;
}
