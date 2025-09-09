#include<stdio.h>
int add(int a,int b,int c)
{
   return(a+b+c);
}
int sub(int a,int b,int c)
{
  return (a-b-c);
}
int main()
{
   int (*fptr)(int,int,int);
   fptr=&add;
   int x=fptr(10,3,2);
   printf("%d",x);
   fptr=sub;
   x=fptr(19,3,2);
   printf("%d",x);
}













