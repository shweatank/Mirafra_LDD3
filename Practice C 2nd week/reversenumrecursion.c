#include<stdio.h>
int reverse(int num)
{
	static int rev=0;
	int r;
	if(num)
	{
		r=num%10;
		rev=rev*10+r;
		reverse(num/10);
	}
	else
		return rev;
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	printf("Reverse = %d\n",reverse(num));
}
