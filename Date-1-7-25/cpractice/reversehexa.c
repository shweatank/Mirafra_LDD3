#include <stdio.h>

int main() 
{
    unsigned int num = 0x12345678;
    unsigned int result;

    result = ((num & 0xFF000000) >> 24) |((num & 0x00FF0000) >> 8)|((num & 0x0000FF00) << 8)|((num & 0x000000FF) << 24);

    printf("Original: 0x%08X\n", num);
    printf("Reversed: 0x%08X\n", result);

    return 0;
}

