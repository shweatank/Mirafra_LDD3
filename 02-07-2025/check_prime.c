#include<stdio.h>
int main()
{
	int num,cnt=0;
	printf("Enter a number to check whether it is prime or not:");
	scanf("%d",&num);

	for(int i=1;i<=num;i++)
	{
		if((num%i)==0)
		{
			cnt++;
		}
	}
	if(cnt==2)
	{
		printf("%d is a prime number\n",num);
	}
	else
	{
		printf("%d is not a prime number\n",num);
	}
	return 0;
}
