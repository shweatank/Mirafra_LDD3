#include<stdio.h>
#include<string.h>
void remove_spaces(char *str)
{
	int len=strlen(str);
	for(int i=0;i<len;i++)
	{
		if(str[i]==' ')
		{
			memmove(str+i,str+i+1,len-i);
			i--;
			len--;
		}
	}
	str[len]='\0';
}
int main()
{
	char str[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	str[strlen(str)-1]='\n';
	str[strlen(str)-1]='\0';
	remove_spaces(str);
	printf("str:%s\n",str);
}
