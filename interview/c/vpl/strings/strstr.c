#include<stdio.h>
#include<string.h>
char *mystrstr(char *str,char *sub)
{
	char *p,*q;
	while(*str)
	{
		p=str;
		q=sub;
		while(*q && *q==*p)
		{
			p++;
			q++;
		}
		if(*q=='\0')
			return str;
		str++;
	}
	return NULL;
}
int main()
{
	char str[100],str1[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	printf("enter str1:\n");
	fgets(str1,100,stdin);
	if(str1[strlen(str1)-1]=='\n')
		str1[strlen(str1)-1]='\0';
	char *p=mystrstr(str,str1);
	if(p!=NULL)
		printf("sub str found at:%ld\n",p-str);
	else
		printf("sub str not found\n");
	
}
