#include<stdio.h>
int bin_dec(int num)
{
        int base=1,res=0;
        int digit;
        while(num)
        {
                digit=num%10;
                if(digit==1)
                        res+=base;
                base*=2;
                num/=10;
        }
        return res;
}
int main()
{
        int num,res;
        printf("enter a bin number:\n");
        scanf("%d",&num);
        res=bin_dec(num);
        printf("decimal:%d\n",res);
}
