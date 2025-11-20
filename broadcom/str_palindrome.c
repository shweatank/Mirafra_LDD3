#include<stdio.h>
#include<string.h>
int is_palin(char *str)
{
	int len=strlen(str);
	int i=0,j=len-1;
	for(;i<j;i++,j--)
	{
		if(str[i]!=str[j])
			return 0;
	}
	return 1;
}
int main()
{
	char str[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	if(is_palin(str))
		printf("yes\n");
	else
		printf("no\n");
}
