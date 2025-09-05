#include<stdio.h>
int add(int x,int y)
{
     return x+y;
}
int sub(int x,int y)
{
	return x-y;
}
int mul(int x,int y)
{
	return x*y;
}
int main()
{
	int ch;
	int (*ptr[3])(int,int)={add,sub,mul};
	printf("enter the choice:\n");
	scanf("%d",&ch);
	printf("result:%d",ptr[ch](10,20));
}
