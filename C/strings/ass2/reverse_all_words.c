#include<stdio.h>
#include<string.h>
void reverse(char *str,int len)
{
	int i=0,j=len-1;
	char ch;
	for(;i<j;i++,j--)
	{
		ch=str[i];
		str[i]=str[j];
		str[j]=ch;
	}
}
void reverse_all_words(char *str)
{
	int len1=strlen(str),len2;
	reverse(str,len1);
	char *p;
	for(p=str;p=strtok(p," ");p=NULL)
	{
		len2=strlen(p);
		reverse(p,len2);
	}
	for(int i=0;i<len1;i++)
		if(str[i]=='\0')
			str[i]=' ';
}
int main()
{
	char str[50];
	printf("enter str1:\n");
	fgets(str,50,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	reverse_all_words(str);
	printf("string after reversing words:%s\n",str);
}
