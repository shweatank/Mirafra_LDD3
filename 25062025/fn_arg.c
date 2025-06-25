/*** function pointer as an argument ***/

#include <stdio.h>

int add(int a,int b)
{
   return a + b;
}
int sub(int a,int b)
{
   return a - b;
}
int calc(int a,int b,int (*fptr)(int,int))
{
   printf("%d \r\n",fptr(a,b));
}
int main()
{
   calc(10,5,add);
   calc(20,4,sub);
   return 0;
}
