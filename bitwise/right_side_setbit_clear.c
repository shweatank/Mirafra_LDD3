#include<stdio.h>
void print_binary(int num)
{
	for(int bit=31;bit>=0;bit--)
	{
		printf("%d",(num>>bit)&1);
		if(bit%8==0)
			printf(" ");
	}
	printf("\n");
}
int main()
{
	int num,pos=-1;
	printf("enter num and bit:");
	scanf("%d",&num);
	print_binary(num);
	num=num&(num-1);
	print_binary(num);
}
