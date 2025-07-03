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
int to_swap(int num,int bit1,int bit2)
{
	int b1,b2;
	b1=(num>>bit1)&1;
	b2=(num>>bit2)&1;
	if(b1!=b2)
	{
		num^=(1<<bit1)|(1<<bit2);
	}
	return num;

}
int main()
{
	int num,bit1,bit2;
	printf("enter num and two bits:");
	scanf("%d%d%d",&num,&bit1,&bit2);
	print_binary(num);
	num=to_swap(num,bit1,bit2);
	print_binary(num);
}
