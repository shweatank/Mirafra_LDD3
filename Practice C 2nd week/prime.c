#include<stdio.h>
int prime(int num)
{
	int i;
	for(i=2;i<num;i++)
		if(num%i==0)
			return 0;
	if(num==i)
		return 1;
	else 
		return 0;
}
void main()
{
	int num;
	printf("Enter the number: ");
	scanf("%d",&num);
	if(prime(num))
		printf("Yes...\n");
	else
		printf("No...\n");
}

