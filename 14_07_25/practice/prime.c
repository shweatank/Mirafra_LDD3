#include<stdio.h>
void main()
{
	int num=100,i;
	for(i=2;i<=100;i++)
	{
		int flag=0;
		for(int j=2;j<i;j++)
		{
			if(i==1)
			{
				printf("1 is neither composite nor prime\n");
			}
			if(i%j==0)
			{
				flag=1;
				break;
			}
		}
		if(flag==0)
		{
			printf("%d is a prime number\n",i);
		}
	}
}

