#include<stdio.h>
int main()
{
	int a,b;
	printf("Enter two values for a and b:");
	scanf("%d %d",&a,&b);
	printf("Before swapping\n");
	printf("a:%d b:%d\n",a,b);
	a=a^b;
	b=a^b;
	a=a^b;
	printf("After Swapping\n");
	printf("a:%d b:%d\n",a,b);
}
