
#include<stdio.h>
#define swapnibble(n)  ((n&0xF00F)|(((n&0x00F0)<<4)|((n&0x0F00)>>4)))
int main()
{
    unsigned short num=0x1234;
    unsigned short swapped=swapnibble(num);
    printf("before swap:0x%04X\n",num);
    printf("after swap:0x%04X\n",swapped);
}
