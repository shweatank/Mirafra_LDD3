#include<stdio.h>

int main()
{
	int i;
	scanf("%d",&i);
	int n=0,count =0,it=0;
	while(n<=31)
	{
		if(((i>>n)&1) ==0)
		{
			if((i>>(++n)&1) ==0)
			{
				count+=2;
			}
			else
			{
				count++;
			}
		}
		++n;
		it++;

	}
	printf("count %d itrations %d\n",count,it);
}
