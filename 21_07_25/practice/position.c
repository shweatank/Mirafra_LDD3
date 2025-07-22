#include<stdio.h>
void main()
{
	int num,pos,i;
	printf("Enter a number ");
	scanf("%d",&num);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	printf("Enter your position ");
	scanf("%d",&pos);
	printf("Enter i value ");
	scanf("%d",&i);
	int result=num&((1<<pos+i)-1);
	result=result>>i-1;
	for(int i=31;i>=0;i--)
		printf("%d",result>>i&1);
	printf("\n");
	printf("%d",result);
	printf("\n");
}
