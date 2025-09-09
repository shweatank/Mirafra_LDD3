#include<stdio.h>
//defining actual functions
int add(int a,int b)
{
  return (a+b);
}
int sub(int a,int b)
{
    return (a-b);
}
struct ops
{
   char name[10];
   int(*fptr)(int,int);
};
int main()
{
   int x;
   struct ops v[3];//creating array of ops structure
   v[1].name[10]="add";
   v[1].fptr=add;
   x=v[1].fptr(10,2);
   printf("%d\n",x);
   
   
}
   
