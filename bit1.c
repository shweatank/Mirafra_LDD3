//24 June 2025
//program to check if the number is power of 2 or not
#include<stdio.h>
int checkpowerof2(int n)
{
	int count=0;
	for(int i=0;i<32;i++)
	{
		if((n>>i)&1)
			count++;
	}
	printf("Count = %d\n",count);
	if(count==1)
		return 1;
	else
		return 0;
}
void main()
{
	int n;
	printf("Enter the number: ");
	scanf("%d",&n);
	if(checkpowerof2(n))
		printf("Yes...\n");
	else
		printf("No...\n");
}
