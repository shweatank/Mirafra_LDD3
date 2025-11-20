#include<stdio.h>
#include<string.h>
int main()
{
	char str[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
}
