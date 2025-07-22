#include<stdio.h>
void main()
{
	int num,count=0;
	printf("Enter a number ");
	scanf("%d",&num);
	for(int i=0;i<=31;i++)
	{
		if((num&(1<<i))!=0)
		{
			count++;
		}
	}
	printf("%d\n",count);
}
