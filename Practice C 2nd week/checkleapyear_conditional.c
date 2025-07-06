#include<stdio.h>
void main()
{
	int y;
	printf("Enter the year : ");
	scanf("%d",&y);
	int r=(y%4)?0:(y%100)?1:(y%400)?0:1;
	if(r)
		printf("Yes...\n");
	else
		printf("No...\n");
}
