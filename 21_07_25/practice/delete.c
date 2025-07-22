#include<stdio.h>
void main()
{
	int num,pos,count;
	printf("Enter a number ");
	scanf("%d",&num);
	for(int i=31;i>=0;i--)
		printf("%d",num>>i&1);
	printf("\n");
	printf("Enter position ");
	scanf("%d",&pos);
	printf("Enter how many bits you want to delete ");
	scanf("%d",&count);
	int left=num>>(pos+count);
	int right=num&((1<<pos)-1);
	int left_shifted=left<<pos;
	int result=right|left_shifted;
	for(int i=31;i>=0;i--)
		printf("%d",result>>i&1);
	printf("\n");
}
