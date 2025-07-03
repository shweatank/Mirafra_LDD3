#include<stdio.h>
int main()
{
	int num1,num2;
	printf("enter two intgers :");
	scanf("%d%d",&num1,&num2);
	printf("before swap:%d %d\n",num1,num2);
	num1^=num2^=num1^=num2;
	printf("after swap:%d %d\n",num1,num2);
}
