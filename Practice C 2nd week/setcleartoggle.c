#include<stdio.h>
void main()
{
	int num,pos;
	printf("enter the number and bit position : ");
	scanf("%d%d",&num,&pos);
	printf("Set : %d\n",num|(1<<pos));
	printf("Clear : %d\n",num&~(1<<pos));
	printf("Toggle : %d\n",num^(1<<pos));
}

