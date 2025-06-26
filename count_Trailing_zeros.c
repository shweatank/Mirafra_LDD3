#include<stdio.h>
int main()
{
	int num,cnt=0;
	printf("Enter the number\n");
	scanf("%d",&num);
	for(int bit=0;bit<32;bit++)
	{
		if((num>>bit)&1)
			break;
		cnt++;
	}
	printf(" trailing zeros are: %d\n",cnt);
}
