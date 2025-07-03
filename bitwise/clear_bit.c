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
	int num,bit;
	printf("enter num and bit:");
	scanf("%d%d",&num,&bit);
	print_binary(num);
	num&=~(1<<bit);
	print_binary(num);
}
