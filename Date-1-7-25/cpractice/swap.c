#include <stdio.h>

int main()
{
    unsigned short value = 0x1234;
    unsigned short n1 = (value & 0xF000);
    unsigned short n2 = (value & 0x0F00) >> 8;
    unsigned short n3 = (value & 0x00F0) >> 4;
    unsigned short n4 = (value & 0x000F);
    unsigned short result = (n1) | (n3 << 8) | (n2 << 4) | n4;
    printf("0x%04X\n", result);
    return 0;
}

