#include<stdio.h>
int sumofdigit(int num)
{
	if(num)
		return (num%10)+sumofdigit(num/10);
	else
		return 0;
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	printf("Sum of digits = %d\n",sumofdigit(num));
}
