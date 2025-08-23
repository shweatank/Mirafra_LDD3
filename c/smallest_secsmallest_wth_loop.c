#include<stdio.h>
void to_check(int num,int *small,int *secsmall)
{
	if(num==0)
		return;
	int digit=num%10;
	if(digit<*small)
	{
		*secsmall=*small;
		*small=digit;

	}
	else if(digit!=*small && digit<*secsmall)
	       *secsmall=digit;
    to_check(num/10,small,secsmall);	
}
int main()
{
	int num,digit,small=9,secsmall=9;
	scanf("%d",&num);
	to_check(num,&small,&secsmall);
	if(secsmall==9)
	printf("smallest :%d and there is no secsmallest",small);
	else
		printf("smallest :%d secsmallest:%d",small,secsmall);
}
