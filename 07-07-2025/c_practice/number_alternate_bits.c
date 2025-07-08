#include<stdio.h>
int alternate_bits(int num)
{
	int res=num^(num>>1);
	if((res&(res+1))==0)
		return 1;
	return 0;
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	if(alternate_bits(num))
		printf("yes num has alternate bits\n");
	else
		printf("num not have alternate bits\n");
}
