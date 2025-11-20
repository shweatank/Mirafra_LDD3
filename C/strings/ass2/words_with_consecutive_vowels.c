#include<stdio.h>
#include<string.h>
int is_vowel(char ch)
{
	if(strchr("aeiouAEIOU",ch))
		return 1;
	return 0;
}
int consecutive(char *p,int len)
{
	for(int i=0;i<len-1;i++)
	{
		if((is_vowel(p[i])) && (is_vowel(p[i+1])))
			return 1;
	}
	return 0;
}
void consecutive_vowels(char *str)
{
	int len=strlen(str),len2,cnt=0;
	char *p;
	printf("words:");
	for(p=str;p=strtok(p," ");p=NULL)
	{
		len2=strlen(p);
		if(consecutive(p,len2))
		{
			cnt++;
			printf("%s ",p);
		}
	}
	printf("\ncnt:%d\n",cnt);
}
int main()
{
	char str[50];
	printf("enter str:\n");
	fgets(str,50,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	char *str2;
	str2=strdup(str);
	consecutive_vowels(str2);
}
