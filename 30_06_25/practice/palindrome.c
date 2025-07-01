#include<stdio.h>
int main()
{
	int num=0,temp=0,rem=0,rev=0;
	printf("Enter a number ");
	scanf("%d",&num);
	if(num<0)
	{
		printf("Enter a number greater than 0\n");
		return -1;
	}
	temp=num;
	while(num>0)
	{
		rem=num%10;
		rev=rev*10+rem;
		num/=10;
	}
	if(temp==rev)
	{
		printf("Palindrome\n");
	}
	else
	{
		printf("Not palindrome\n");
	}
	return 0;
}
