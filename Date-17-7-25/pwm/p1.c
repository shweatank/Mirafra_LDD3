#include<stdio.h>
int main()
{
	int i;

	for(i=0;i<=1000;i++)
	{
		if((i/10==1)||(i/10==0)||(i%10==1)||(i%10==0))
		printf("%d\n",i);
	}
}
