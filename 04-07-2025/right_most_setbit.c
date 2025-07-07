#include<stdio.h>
int right_most(int num)
{
	if(num==0)
		return -1;
	int mask=1,pos=0;
	while((num & mask)==0)
	{
		mask<<=1;
		pos++;
	}
	return pos;
}
void print_binary(int num)
{
	for(int bit=31;bit>=0;bit--)
		printf("%d",(num>>bit)&1);
	printf("\n");
}
int main()
{
	int num;
	printf("enter a num:\n");
	scanf("%d",&num);
	print_binary(num);
	int pos=right_most(num);
	if(pos!=-1)
		printf("right most set bit:%d\n",pos);
	else
		printf("no set bits in number\n");
}
