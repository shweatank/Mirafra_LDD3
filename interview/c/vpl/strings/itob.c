#include<stdio.h>
#include<string.h>
void rev(char *str)
{
	int i=0,j=strlen(str)-1;
	for(;i<j;i++,j--)
	{
		char ch=str[i];
		str[i]=str[j];
		str[j]=ch;
	}
}
void myitob(int num)
{
	char str[100];
	int i=0;
	while(num)
	{
		str[i++]=num%2+'0';
		num=num/2;
	}
	str[i]='\0';
	rev(str);
	printf("str:%s\n",str);
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	myitob(num);
}
