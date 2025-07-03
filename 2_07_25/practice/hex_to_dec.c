#include<stdio.h>
#include<string.h>
void main()
{
	char hex[32]={0};
	int i=0,dec=0,j=1;
	printf("Enter a hexadecimal number ");
	scanf("%s",hex);
	int len=strlen(hex);
	for(i=len-1;hex[i];i--)
	{
		if(hex[i]>='0'&&hex[i]<='9')
		{
			dec+=(hex[i]-'0')*j;
			j*=16;
		}
		else if(hex[i]>='A'&&hex[i]<='F')
		{
			dec+=(hex[i]-'A'+10)*j;
			j*=16;
		}
		else if(hex[i]>='a'&&hex[i]<='f')
		{
			dec+=(hex[i]-'a'+10)*j;
			j*=16;
		}
	}
	printf("%s of decimal is %d\n",hex,dec);
}
