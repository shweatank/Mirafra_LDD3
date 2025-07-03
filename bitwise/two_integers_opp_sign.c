#include<stdio.h>
int main()
{
	int num1,num2;
	printf("enter two integers");
	scanf("%d%d",&num1,&num2);
	if((num1 ^ num2)<0)
		printf("yes two integers has opposite sign\n");
	else
		printf(" two integers has same sign\n");
}
