#include<stdio.h>
#include<string.h>
int is_vowel(char ch)
{
	if(strchr("aeiouAEIOU",ch))
		return 1;
	return 0;
}
int hide_pair(char *str)
{
	int found=0;
	int len=strlen(str);
	for(int i=0;i<len-1;i++)
	{
		if(is_vowel(str[i]) && is_vowel(str[i+1]))
		{
			str[i]='*';
			str[i+1]='*';
			i++;
			found=1;
		}
	}
	return found;
}
int main()
{
	char str[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	if(hide_pair(str))
	printf("str:%s\n",str);
	else
		printf("no pair of vowels\n");
}
