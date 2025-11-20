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
void longest(int num)
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
				pos=bit+cnt-1;
			}
		}
		else
			cnt=0;
	}
	printf("cnt:%d pos:%d\n",cnt1,pos);
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	print_binary(num);
	longest(num);
}
