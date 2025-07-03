#include<stdio.h>
void main()
{
	int dec=0,bin[32]={0},rem=0,j=0;
	printf("Enter a number in decimal form ");
	scanf("%d",&dec);
	while(dec>0)
	{
		rem=dec%2;
		bin[j]=rem;
		j++;
		dec/=2;
	}
	int len=sizeof(bin)/sizeof(bin[0]);
	for(int i=0;i<len/2;i++)
	{
		int a=bin[i];
		bin[i]=bin[len-1-i];
		bin[len-1-i]=a;
	}
	for(int i=0;i<len;i++)
	{
		printf("%d ",bin[i]);
	}
}

