#include <stdio.h>
void print_binary(float num)
{
    unsigned int *ptr=(unsigned int *)&num; 
    unsigned int n=*ptr;
    for (int bit=31;bit>=0;bit--)
    {
        printf("%d", (n>>bit)&1);
    }
    printf("\n");
}

int main() 
{
    float num;
    printf("Enter float number:\n");
    scanf("%f", &num);
    print_binary(num);
    return 0;
}

