#include<stdio.h>
int strong(int num)
{
	int r,p,sum=0,t;
	for(t=num;t;t/=10)
	{
		r=t%10;
		for(p=1;r;r--)
			p*=r;
		sum+=p;
	}
	if(sum==num)
		return 1;
	else
		return 0;
}
void main()
{
	int num;
	printf("Enter the number : ");
	scanf("%d",&num);
	if(strong(num))
		printf("Yes...It is Strong...\n");
	else
		printf("No...It is not Strong...\n");
}
