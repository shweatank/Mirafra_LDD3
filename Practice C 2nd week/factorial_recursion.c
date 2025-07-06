#include<stdio.h>
int fact(int num)
{
	if(num)
		return num*fact(num-1);
	else
		return 1;
}
void main()
{
	int num;
	printf("enter the number: ");
	scanf("%d",&num);
	printf("%d\n",fact(num));
}
