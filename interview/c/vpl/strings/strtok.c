#include<stdio.h>
#include<string.h>
char *mystrtok(char *str,char*del)
{
	char *p,*q;
	if(str!=NULL)
		p=q=str;
	else
		p=q;
	while(*q)
	{
		if(*q==*del)
		{
			*q='\0';
			q++;
			break;
		}
		q++;
	}
	if(*p=='\0')
		return NULL;
	return p;
}

int main()
{
	int cnt=0;
	char str[100];
	printf("enter str:\n");
	fgets(str,100,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	char *p;
	for(p=str;p=mystrtok(p," ");p=NULL)
		cnt++;
	printf("cnt:%d\n",cnt);
}
