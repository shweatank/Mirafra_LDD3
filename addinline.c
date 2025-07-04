#include<stdio.h>

int op(int a,int b)
{
	int c,op;
	printf("enter op to be perform\n");
	scanf("%d",&op);
	switch(op)
	{
		case 1:c=a+b;
		       break;
		case 2:c=a-b;
                       break;
		case 3:c=a*b;
                       break;
		case 4:c=a/b;
                       break;
		default:
		       break;
	}
	return c;
}

int main()
{
	int a,b;
	scanf("%d",&a);
	scanf("%d",&b);
	printf("result =%d\n",op(a,b));
}
