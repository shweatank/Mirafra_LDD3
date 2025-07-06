#include<stdio.h>
void main()
{
	int num;
	printf("Enter the number:");
	scanf("%d",&num);
	//num^=0xffffffff;
	num=-(~num);
	printf("Result = %d\n",num);
}
