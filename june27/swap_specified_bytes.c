#include<stdio.h>
int swap_bytes(int num,int b1,int b2)
{
        int byte1,byte2;
        byte1=((num>>(b1*8))&0xff);
        byte2=((num>>(b2*8))&0xff);

        num&=~(0xff<<(b1*8));
        num&=~(0xff<<(b2*8));

        num|=(byte1<<(b2*8));
        num|=(byte2<<(b1*8));

        return num;
}

int main()
{
        int num,b1,b2;
        printf("enter num and bytes to swap:\n");
        scanf("%x%d%d",&num,&b1,&b2);
        printf("0x%x\n",num);
        num=swap_bytes(num,b1,b2);
        printf("0x%x\n",num);
}
