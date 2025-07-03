#include<stdio.h>
void main()
{
	int num=0,j=1,rem=0,dec=0;
	printf("Enter a number in binary ");
	scanf("%d",&num);
	while(num>0)
	{
		rem=num%10;
		dec+=rem*j;
		j=j*2;
		num/=10;
	}
	printf("Decimal number %d\n",dec);
}
