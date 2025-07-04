#include<stdio.h>
int rev_bits(int num)
{
	int left=31,right=0;
	while(left>right)
	{
		if(((num>>left)&1)!=((num>>right)&1))
		{
			num^=1<<left;
			num^=1<<right;
		}
		left--;
		right++;
	}
	return num;
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
	printf("enter number:\n");
	scanf("%d",&num);
	printf("before reversing:");
	print_binary(num);
	num=rev_bits(num);
	printf("after reversing:");
	print_binary(num);
}

