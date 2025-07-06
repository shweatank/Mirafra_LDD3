#include<stdio.h>
int power(int num,int pow)
{
	if(pow)
	{
		pow--;
		return num*power(num,pow);
	}
	else
		return 1;
}
void main()
{
	int num,pow;
	printf("Enter the num and power : ");
	scanf("%d%d",&num,&pow);
	printf("%d^%d = %d\n",num,pow,power(num,pow));
}
