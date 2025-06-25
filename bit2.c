//24 June 2025
//program to check if number is multiplied by 8 or not
#include<stdio.h>
int checkx8(int num)
{
	if(((num>>3)<<3)==num)
		return 1;
	else
		return 0;
}
void main()
{
	int num;
	printf("enter the number : ");
	scanf("%d",&num);
	if(checkx8(num))
		printf("Yes...Multiple of 8\n");
	else
		printf("Not multiple of 8...\n");
}
