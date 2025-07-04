#include<stdio.h>
void main()
{
	int num=0,rem=0,i=0;
	char hex[8];
	printf("Enter a number ");
	scanf("%d",&num);
	while(num>0)
	{
		rem=num%16;
		if(rem>=10&&rem<=15)
		{
			hex[i]=(char)(rem+55);
		}
		else
		{
			hex[i]=(char)(rem+48);
		}
		i++;
		num/=16;
	}
	for(i=i-1;i>=0;i--)
	{
		printf("%c",hex[i]);
	}
	printf("\n");
}
