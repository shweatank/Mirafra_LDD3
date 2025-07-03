#include<stdio.h>
int main()
{
	int num1,num2,cnt=0;
	printf("enter two integers:");
	scanf("%d%d",&num1,&num2);
	int res=num1^num2;
	while(res)
	{
		res=res&(res-1);
		cnt++;
	}
	printf("count:%d",cnt);

}
