#include<stdio.h>


int main()
{
	unsigned int n;
	scanf("%d",&n);
	int flag=0;
	{
		if((n&(n-1))==0)
			flag=1;
		else
			flag=0;
	}
	if(flag==1)
		printf("%d is power of 2\n",n);
	else
		printf("%d is not power of 2\n",n);
}
