#include <stdio.h>
#include <stdint.h>

int setBits(uint32_t n, int *positions)
{
    int count = 0;
    int pos = 0;

    while (n)
    {
        if (n & 1)
       	{
            positions[count] = pos; 
            count++;
        }
        n >>= 1;
        pos++;
    }

    return count;
}

int main() 
{
    uint32_t num = 10;  
    int positions[32];  

    int count = setBits(num, positions);

    printf("Set bit count: %d\n", count);
    printf("Set bit positions: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", positions[i]);
    }
    printf("\n");

    return 0;
}
