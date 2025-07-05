#include<stdio.h>
int main()
{
    int n;
    printf("enter a number:\n");
    scanf("%d",&n);
    int max=0,min=9;
    if(n==0)
    {
        min=max=0;
    }
    else
    {
    while(n!=0)
    {
        int r=n%10;
        if(r>max)
        {
            max=r;
            
        }
        if(r<min)
        {
            min=r;
            
        }
        n/=10;
    }
    }
    printf("min=%d\n",min);
    printf("max=%d\n",max);
}
