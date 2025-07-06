#include<stdio.h>
int count(int num)
{
	if(num>0)
		return 1+count(num/10);
	else
		return 0;
}
void main()
{
	int num,c;
	printf("Enter the number: ");
	scanf("%d",&num);
	c=count(num);
	printf("Couunt of digits : %d\n",c);
}
