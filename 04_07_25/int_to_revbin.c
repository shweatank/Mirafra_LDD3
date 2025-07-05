#include<stdio.h>
void print_binary(int n)
{
	int bit=31;
	while(bit>=0)
	{
		printf("%d",(n>>bit)&1);
		if(bit%8==0 && bit!=0)
		   printf(" ");
		bit--;
	}
	printf("\n");
}
int main()
{
	int num,i=31,j=0;
	printf("enter the num:\n");
	scanf("%d",&num);
	printf("the number before revesre:%d\n",num);
	print_binary(num);
	while(i>j)
	{
		if(((num>>i)&1)!=((num>>j)&1))
		{
			num=num^(1<<i);
			num=num^(1<<j);
		}
		i--;
		j++;
	}
	printf("the number after reversing:%d\n",num);
	print_binary(num);
}

