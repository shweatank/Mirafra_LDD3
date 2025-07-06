#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int * dectobin(int num)
{
	int *arr=calloc(32,sizeof(int));
	int i=0;
	while(num)
	{
		arr[i++]=num%2;
		num/=2;
	}
	return arr;
}
void main()
{
	int num,i;
	printf("Enter the number : ");
	scanf("%d",&num);
	int *arr=dectobin(num);
	for(i=31;i>=0;i--)
	{
		printf("%d",arr[i]);
		if(i%8==0)
			printf(" ");
	}
}
