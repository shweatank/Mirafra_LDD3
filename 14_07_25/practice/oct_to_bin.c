#include<stdio.h>
void main()
{
	int oct=0,dec=0,rem=0,j=1,i=1,bin=0;
	printf("Enter a number in octal ");
	scanf("%d",&oct);
	while(oct>0)
	{
		rem=oct%10;
		dec=dec+rem*j;
		j=j*8;
		oct=oct/10;
	}
	while(dec>0)
	{
		rem=dec%2;
		bin=bin+rem*i;
		i=i*10;
		dec=dec/2;
	}
	printf("%d\n",bin);
}
