#include<stdio.h>
void add(int x,int y)
{
	printf("Result:%d\n",x+y);
}
void sub(int x,int y)
{
	printf("Result:%d\n",x-y);
}
void mul(int x,int y)
{
	printf("Result:%d\n",x*y);
}
void div(int x,int y)
{
	printf("Result:%d\n",x/y);
}
void callbackfunc(int x,int y,void (*op)(int ,int))
{
	op(x,y);
}
int main()
{
	callbackfunc(10,20,add);
	callbackfunc(10,20,sub);
	callbackfunc(10,20,mul);
	callbackfunc(10,20,div);
}
