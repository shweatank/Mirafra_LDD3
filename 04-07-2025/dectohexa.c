#include<stdio.h>
#include<string.h>
void rev(char *str)
{
	int i=0,j=strlen(str)-1;
	char ch;
	for(;i<j;i++,j--)
	{
		ch=str[i];
		str[i]=str[j];
		str[j]=ch;
	}
}
void dectohexa(int num,char *str)
{
	int i=0,rem;
	while(num)
	{
		rem=num%16;
		if(rem<10)
			str[i++]=rem+48;
		else
			str[i++]=rem-10+'A';
		num/=16;
	}
	str[i]='\0';
	rev(str);
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	char str[100];
	dectohexa(num,str);
	printf("hexa:0x%s\n",str);
}
