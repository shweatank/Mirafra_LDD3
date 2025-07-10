//Swapping without another variable


#include<stdio.h>
int main()
{
	int a,b;
	printf("Enter two values to swap:");
	scanf("%d %d",&a,&b);

	printf("Before swapping:\n");
	printf("a=%d b=%d\n",a,b);

	a=a+b;
	b=a-b;
	a=a-b;

	printf("After swapping:\n");
	printf("a=%d b=%d\n",a,b);
}
