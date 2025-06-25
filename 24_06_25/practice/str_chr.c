#include<stdio.h>
void remove_newline(char *s)
{
	int i=0;
	while(s[i])
	{
		if(s[i]=='\n'||s[i]=='\0')
		{
			s[i]='\0';
			break;
		}
		i++;
	}
}
char *str_chr(char *s,int c)
{
	while(*s)
	{
		if(*s==(char)c)
		{
			return (char *)s;
		}
		s++;
	}
	return NULL;
}
int main()
{
	char str[32];
	char ch;
	printf("Enter a string ");
	fgets(str,sizeof(str),stdin);
	remove_newline(str);
	char *result=str_chr(str,'w');
	if(result)
	{
		printf("Found character at %s\n",result);
	}
	else
	{
		printf("Not found\n");
	}
	return 0;
}
