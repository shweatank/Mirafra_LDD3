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
	int num;
	printf("enter num and bit:");
	scanf("%d",&num);
	print_binary(num);
	if((num&(num-1))==0)
		printf("yes");
	else
		printf("no");
}
