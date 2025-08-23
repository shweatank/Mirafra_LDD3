#include<stdio.h>
int main()
{
	int num,digit,small=9,secsmall=9;
	scanf("%d",&num);
	while(num)
	{
		digit=num%10;
		if(digit<small)
		{
			secsmall=small;
			small=digit;

		}
		else if(digit!=small && digit<secsmall)
		{
			secsmall=digit;
		}
		num=num/10;
	}
	if(secsmall==9)
	printf("smallest :%d and there is no secsmallest",small);
	else
		printf("smallest :%d secsmallest:%d",small,secsmall);
}
