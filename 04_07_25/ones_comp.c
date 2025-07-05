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
	int num,bit=31;
	printf("enter the  um\n");
	scanf("%d",&num);
	printf("number before 1's compliment:%d\n",num);
	print_binary(num);
	while(bit>=0)
	{
		if((num>>bit)&1)
		{
			num=num^(1<<bit);
		}
		else
		{
			num=num^(1<<bit);
		}
		bit--;
	}
	printf("number after 1's compliment:%d\n",num);
	print_binary(num);
}
