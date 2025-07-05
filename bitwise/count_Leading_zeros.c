#include<stdio.h>
int main()
{
	int num,cnt=0;
	printf("Enter a number\n");
	scanf("%d",&num);
	int msb=31;
	for(int bit=0;bit<32;bit++)
	{
		if(((num<<bit)&(1<<msb)))
		{
			break;
		}
		cnt++;
	}	
	printf("leading zeros are :%d\n",cnt);
}
