#include<stdio.h>
int fun(int n)
{
        if(n==0)
                return 0;
        else
                return n+fun(n-1);
}
int main()
{
        int n;
        printf("enter n value:\n");
        scanf("%d",&n);
        int sum=fun(n);
        printf("%d\n",sum);
}
