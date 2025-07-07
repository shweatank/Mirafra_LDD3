#include<stdio.h>
int count_flips(int num1,int num2)
{
	int num=num1^num2;
	int cnt=0;
	while(num)
	{
		cnt++;
		num&=(num-1);
	}
	return cnt;
}
int main()
{
	int num1,num2;
	printf("enter 2 numbers:\n");
	scanf("%d%d",&num1,&num2);
	int cnt=count_flips(num1,num2);
	printf("count flips:%d\n",cnt);
}
