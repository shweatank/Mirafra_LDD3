#include<stdio.h>
void add(int x,int y)
{
	printf("sum:%d",x+y);
}
void sub(int x,int y)
{
	printf("sub:%d",x-y);
}
int main()
{
	char ch;
	void (*ptr)(int,int);
	printf("enter the choice:\n");
	scanf("%c",&ch);
	if(ch=='+')
		ptr=add;
	else if(ch=='-')
		ptr=sub;
	else
	{
		printf("enter a valid choice\n");
		return 0;
	}
        ptr(10,20);
}
