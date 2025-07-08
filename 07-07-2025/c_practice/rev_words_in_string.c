#include<stdio.h>
#include<string.h>
void rev(char *str)
{
	char ch;
	int i=0,j=strlen(str)-1;
	for(;i<j;i++,j--)
	{
		ch=str[i];
		str[i]=str[j];
		str[j]=ch;
	}
}
void rev_fun(char *str)
{
	rev(str);
	int len=strlen(str);
	char *p=NULL;
	for(p=str;p=strtok(p," ");p=NULL)
	{
		rev(p);
	}
	for(int i=0;i<len;i++)
	{
		if(str[i]=='\0')
			str[i]=' ';
	}
}
int main()
{
	char str[100];
	printf("enter a str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	rev_fun(str);
	printf("str after rev:%s\n",str);
}
