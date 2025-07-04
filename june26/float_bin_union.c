#include <stdio.h>
typedef union 
{
     float f;
     unsigned int u;
} un;
void print_binary(un var)
{

    for (int bit=31;bit>=0;bit--)
        printf("%d", (var.u >> bit) & 1);
    printf("\n");
}

int main()
{
    un var;
    printf("Enter float number:\n");
    scanf("%f", &var.f);
    print_binary(var);
    return 0;
}

