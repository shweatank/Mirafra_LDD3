#include<stdio.h>
int add(int a,int b)
{
   return(a+b);
}
int calc(int a,int b,int(*fptr)(int,int))
{
     return  fptr(a,b);
}
int main()
{
   printf("%d\n",calc(10,2,add));
}
