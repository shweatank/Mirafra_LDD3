#include<stdio.h>
void main()
{
	int num=0,rem=0,j=1,oct=0,dec=0;
	printf("Enter a number in(0's and 1's only) ");
	scanf("%d",&num);
	while(num>0)
	{
		rem=num%10;
		dec+=rem*j;
		j*=2;
		num/=10;
	}
	j=1;
	printf("decimal number is %d\n",dec);
	while(dec>0)
	{
		rem=dec%8;
		oct+=rem*j;
		j*=10;
		dec/=8;
	}
	printf("octal number is %d\n",oct);
}
