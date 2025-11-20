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
int pair_setbits(int num)
{
	int cnt=0;
	for(int bit=31;bit>0;bit--)
	{
		if(((num>>bit)&1) && ((num>>(bit-1))&1))
		{
			cnt++;
			bit--;
		}
	}
	return cnt;
}

int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	print_binary(num);
	int cnt=pair_setbits(num);
	printf("cnt:%d\n",cnt);
}

