#include<stdio.h>
#include<string.h>
void remove_dup(char *str)
{
	int len=strlen(str);
	int index=0,j;
	for(int i=0;i<len;i++)
	{
		for(j=0;j<index;j++)
		{
			if(str[i]==str[j])
				break;
		}
		if(j==index)
			str[index++]=str[i];
	}
	str[index]='\0';
}
int main()
{
	char str[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	remove_dup(str);
	printf("str after remove:%s\n",str);
}
