#include<stdio.h>
void print_binary(unsigned int num)
{
	for(int bit=31;bit>=0;bit--)
	{
		printf("%d",(num>>bit)&1);
		if(bit%8==0)
			printf(" ");
	}
	printf("\n");

}
int to_set(unsigned int num,int i,int j)
{
	num^=((1<<(j-i+1))-1)<<i;
	return num;
}
int main()
{
	unsigned int num;
	int i,j;
	printf("enter num ,i and j:");
	scanf("%d%d%d",&num,&i,&j);
	print_binary(num);
	num=to_set(num,i,j);
	print_binary(num);
}
