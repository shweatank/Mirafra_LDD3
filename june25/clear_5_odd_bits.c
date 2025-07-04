#include<stdio.h>
int printbinary(int data)
{
        int bit;
        for(bit=31;bit>=0;bit--)
        {
                printf("%d",(data>>bit)&1);
        }
}
int main()
{
        int num,bit;
        scanf("%d",&num);
        printbinary(num);
        for(bit=9;bit>=0;bit--)
        {
                if((bit%2)!=0)
                {
                        num&=(1<<bit);
                }
        }
                printf("%d",num);
                printbinary(num);
}
