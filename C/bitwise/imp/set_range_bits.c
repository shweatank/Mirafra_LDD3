#include<stdio.h>
void print_binary(int num)
{
	for(int i=31;i>=0;i--)
	{
		printf("%d",(num>>i)&1);
		if(i%8==0)
			printf(" ");
	}
	printf("\n");
}
int set_range(int num,int i,int j)
{
	int mask=0;
	if(i==0 && j==31)
		mask=0xFFFFFFFF;
	else
		mask=((1<<(j-i+1))-1)<<i;
	return num|mask;
}
int main()
{
	int num,i,j;
	printf("enter num and range:\n");
	scanf("%d%d%d",&num,&i,&j);
	print_binary(num);
	num=set_range(num,i,j);
	print_binary(num);
}
