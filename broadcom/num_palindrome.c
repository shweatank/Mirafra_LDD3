#include<stdio.h>
int is_palin(int num)
{
	if(num<0)
		return 0;
	int temp=num,res=0;
	while(num)
	{
		res=res*10+(num%10);
		num/=10;
	}
	if(res==temp)
		return 1;
	return 0;
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	if(is_palin(num))
		printf("yes");
	else
		printf("no");
}
