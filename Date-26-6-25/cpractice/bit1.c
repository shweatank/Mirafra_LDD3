#include<stdio.h>
int main()
{
    int result = 0;
    int a=30;
    int b=6;
    int sign = ((a<0)^(b<0))?-1:1;
    while(a>=b)
    {
        long long temp1=b;
        long long temp2=1;
        while(a>=(temp1<<1))
        {
            temp1=temp1<<1;
            temp2=temp2<<1;
        }
        a=a-temp1;
        result=result+temp2;
    }
    printf("the divison is %d\n",sign*result);
}
