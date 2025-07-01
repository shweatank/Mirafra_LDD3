#include<stdio.h>
#include<math.h>
int isperfectsquare(int n)
{
	int sr=0;
	sr=sqrt(n);
	if(n>=0)
	{
		return (sr*sr==n);
	}
	return 0;
}
int main()
{
	int num=0;
	printf("Enter a number ");
	scanf("%d",&num);
	if(isperfectsquare(num)==1)
	{
		printf("True\n");
	}
	else
	{
		printf("False\n");
	}
	return 0;
}
