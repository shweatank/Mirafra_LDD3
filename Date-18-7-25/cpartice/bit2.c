//set a bits in hex 
#include<stdio.h>
int main()
{
	short int num=0x1234;
	int i,j;
	printf("Enter The i and j:\n");
	scanf("%d%d",&i,&j);

	int mask=((1<<(j-i+1))-1)<<i;
	num=mask|num;

	printf("num=0x%0x\n",num);
}

