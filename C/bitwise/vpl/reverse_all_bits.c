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

int reverse(int num)
{
	int l=31,r=0;
	while(l>r)
	{
		if(((num>>l)&1)!=((num>>r)&1))
		{
			num^=(1<<l);
			num^=(1<<r);
		}
		l--;
		r++;
	}
	return num;
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	print_binary(num);
	num=reverse(num);
	print_binary(num);
}
