#include<stdio.h>
#include<string.h>
void reverse(char *p,int len)
{
	char ch;
	int i=0,j=len-1;
	for(;i<j;i++,j--)
	{
		ch=p[i];
		p[i]=p[j];
		p[j]=ch;
	}
}
void reverse_word(char *str1,char *str2)
{
	char *p;
	int len=strlen(str1);
	for(p=str1;p=strtok(p," ");p=NULL)
	{
		int len=strlen(p);
		if(strcmp(p,str2)==0)
			reverse(p,len);
	}
	for(int i=0;i<len;i++)
		if(str1[i]=='\0')
			str1[i]=' ';
}

int main()
{
	char str1[50],str2[50];
	printf("enter str1:\n");
	fgets(str1,50,stdin);
	if(str1[strlen(str1)-1]=='\n')
		str1[strlen(str1)-1]='\0';
	printf("enter str2:\n");
	fgets(str2,50,stdin);
	if(str2[strlen(str2)-1]=='\n')
		str2[strlen(str2)-1]='\0';
	reverse_word(str1,str2);
	printf("str1 after reversing word %s:%s\n",str2,str1);
}
