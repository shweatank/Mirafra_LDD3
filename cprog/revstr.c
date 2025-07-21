#include<stdio.h>
#include<string.h>
int i=0;
void reverse(char *str,char *rev)
{
	if(*str != '\0')
	{
		reverse(str+1,rev);
		rev[i++]=*str;
	}
}
void wordrev(char *s,int l)
{
	for(int i=0,j=l-1;i<j;i++,j--)
	{
		char temp;
		temp=s[i];
		s[i]=s[j];
		s[j]=temp;
	}
}
int main()
{
	char str[50],rev[500];
	fgets(str,50,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	reverse(str,rev);
	rev[i]='\0';
	char *temp=rev;
        int count=0;
	for(int i=0;i<=strlen(rev);i++)
	{
		if((rev[i] != ' ')&&(rev[i]!='\0'))
		{
			count++;
		}
		else
		{
			wordrev(temp,count);
			temp=temp+count+1;
			count=0;
		}
	}
	puts(rev);
}
