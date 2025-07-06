#include<stdio.h>
int sum(int lower,int upper)
{
	if(lower<=upper)
		return lower+sum(lower+1,upper);
	else
		return 0;
}
void main()
{
	int lower,upper;
	printf("Enter the lower and upper limits : ");
	scanf("%d%d",&lower,&upper);
	printf("Sum = %d\n",sum(lower,upper));
}
