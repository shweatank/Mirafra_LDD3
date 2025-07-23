#include<stdio.h>
void print_binary(int *p)
{
	for(int i=31;i>=0;i--)
		printf("%d",*p>>i&1);
	printf("\n");
}
int main()
{
	int num;
	printf("Enter The NUmber:\n");
	scanf("%d",&num);
	print_binary(&num);
	int temp=((num&0x55555555)<<1)|((num&0xAAAAAAAA)>>1);
	print_binary(&temp);

}
