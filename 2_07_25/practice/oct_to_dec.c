#include<stdio.h>
void main()
{
	int num=0,dec=0,j=1;
	printf("Enter a number in octal ");
	scanf("%d",&num);
	int temp=num;
	while(num>0)
	{
		int rem=num%10;
		dec+=rem*j;
		j=j*8;
		num/=10;
	}
	printf("%d of decimal number is %d\n",temp,dec);
}
