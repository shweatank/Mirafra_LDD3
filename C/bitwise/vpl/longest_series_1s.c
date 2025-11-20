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

void longest_series(int num)
{
	int cnt=0,cnt1=0,pos=0;
	for(int bit=31;bit>=0;bit--)
	{
		if((num>>bit)&1)
		{
			cnt++;
			if(cnt>cnt1)
			{
				cnt1=cnt;
				pos=cnt+bit-1;
			}
		}
		else
			cnt=0;
	}
	printf("longest series of 1s:%d\n",cnt1);
	printf("position:%d\n",pos);
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	print_binary(num);
	longest_series(num);
}
