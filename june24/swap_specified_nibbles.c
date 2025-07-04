#include<stdio.h>
int swap_nibbles(int num,int n1,int n2)
{
        int nib1,nib2;
        nib1=((num>>(n1*4))&0xf);
        nib2=((num>>(n2*4))&0xf);
        num&=~(0xf<<(n1*4));
        num&=~(0xf<<(n2*4));

        num|=(nib1<<(n2*4));
        num|=(nib2<<(n1*4));
        return num;
}

int main()
{
        int num,n1,n2;
        printf("enter num and nibbles to swap:\n");
        scanf("%x%d%d",&num,&n1,&n2);
        printf("0x%x\n",num);
        num=swap_nibbles(num,n1,n2);
        printf("0x%x\n",num);
}
