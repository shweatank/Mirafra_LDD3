#include<stdio.h>
void remove_newline(char *s)
{
	while(*s)
	{
		if(*s=='\n')
		{
			*s='\0';
			break;
		}
		s++;
	}
}
char *strr_chr(char *s,int c)
{
	char *src=NULL;
	while(*s)
	{
		if(*s==(char)c)
		{
			src=s;
		}
		s++;
	}
	return src;
}
int main()
{
	char str1[32];
	printf("Enter a string ");
	fgets(str1,sizeof(str1),stdin);
	remove_newline(str1);
	printf("%s\n",strr_chr(str1,'l'));
	return 0;
}
