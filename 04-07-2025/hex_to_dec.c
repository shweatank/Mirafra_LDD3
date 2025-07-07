#include<stdio.h>
int hex_to_dec(char *str)
{
	int len=strlen(str);
	for(int i=0;i<len;i++)
	{
		if(str[i]>)
int main()
{
	char str[100];
	printf("enter hex number:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	int res=hex_to_dec(str);
	printf("decimal value:%d\n",res);
}
