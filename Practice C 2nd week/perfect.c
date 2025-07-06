#include<stdio.h>
int perfect(int num)
{
	int i,sum=0;
	for(i=1;i<num;i++)
	{
		if(num%i==0)
			sum+=i;
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
	if(perfect(num))
		printf("Yes...\n");
	else
		printf("No...\n");
}
