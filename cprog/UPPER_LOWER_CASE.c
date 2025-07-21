#include<stdio.h>
#include<string.h>
char mytoupper(char ch)
{
	if(ch>='a' && ch<='z')
	{
		ch^=32;
		return ch;
	}
	return ch;
}
int isupper(char ch)
{
	if(ch>='A' && ch<='Z')
	{
		return 1;
	}
	return 0;
}

int main()
{
	char str[20];
	gets(str);
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i] >= 'a' && str[i]<='z')
		{
			str[i]=mytoupper(str[i]);
		}
		else
		{
			continue;
		}
	}
	puts(str);
}
