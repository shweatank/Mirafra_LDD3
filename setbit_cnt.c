#include<stdio.h>
int main()
{
	int num,bit=31,cnt=0;
	printf("Enter a number:");
	scanf("%d",&num);

	while(bit>=0)
	{
		if(num&(1<<bit))
		{
			cnt++;
		}
		bit--;
	}
	printf("No.of setbits in the given integer are:%d\n",cnt);
	return 0;
}

