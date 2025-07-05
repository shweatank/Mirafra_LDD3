// Replace 0's with 1's in an integer

#include<stdio.h>
int main()
{
	int num,digit,place=1,res=0;
	printf("Enter an integer:");
	scanf("%d",&num);

	while(num)
	{
		digit=num%10;
		if(digit==0)
		{
			digit=1;
		}
		res=res+digit*place;
		place=place*10;
		num=num/10;
	}
	printf("After replacing all zeroes with 1's in the given integer, integer is %d\n",res);
	return 0;
}
