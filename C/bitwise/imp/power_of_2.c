#include<stdio.h>
int main()
{
	int num;
	printf("enter a num:\n");
	scanf("%d",&num);
	if((num&(num-1))==0)
		printf("yes\n");
	else
		printf("No\n");
}
