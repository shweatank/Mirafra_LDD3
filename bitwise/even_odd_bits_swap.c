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
int to_swap(int num)
{
	int num1,num2;
	num1=num&0xAAAAAAAA;
	num2=num&0x55555555;
	return (num1>>1)|(num2<<1);

}
int main()
{
	int num;
	printf("enter num :");
	scanf("%d",&num);
	print_binary(num);
	num=to_swap(num);
	print_binary(num);
}
