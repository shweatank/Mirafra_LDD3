#include<stdio.h>
int palindrome(int num)
{
	static int rev=0;
	int r;
	if(num)
	{
		r=num%10;
		rev=rev*10+r;
		palindrome(num/10);
		if(num==rev)
			return 1;
		else
			return 0;
	}
}
void main()
{
	int num;
	printf("Enter the number :");
	scanf("%d",&num);
	if(palindrome(num))
		printf("yes...\n");
	else
		printf("No...\n");
}
