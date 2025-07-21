#include<stdio.h>

#include<string.h>

int mystrncmp(char *str,char *sub,int n);
char *mystrstr(char *str,char *sub)
{
	int len=strlen(sub);
	while(*str)
	{
		if(mystrncmp(str,sub,len)==0)
		{
			return str;
		}
		else
		{
			str++;
		}
	}
	return NULL;
}
int mystrncmp(char *str,char *sub,int n)
{
	for(int i=0;i<n;i++)
	{
		if(str[i]!=sub[i])
		{
			return (str[i]-sub[i]);
		}
	}
	return 0;
}
int main()
{
	char str[100],sub[10],*temp;
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	fgets(sub,10,stdin);
	if(sub[strlen(sub)-1]=='\n')
		sub[strlen(sub)-1]='\0';
	temp=mystrstr(str,sub);
	if(temp==NULL)
		puts("sub str not found");
	else
	{
		printf("substr found at %ld\n",temp-str);
	}

}
