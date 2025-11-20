#include<stdio.h>
#include<string.h>
char *convert(char ch)
{
	switch(ch)
	{
		case '0':return "zero";
		case '1':return "one";
		case '2':return "two"; 
		case '3':return "three";
		case '4':return "four";
		case '5':return "five";
		case '6':return "six";
		case '7':return "seven";
		case '8':return "eight";
		case '9':return "nine";
	}
}
int main()
{
	char str[50];
	printf("enter str:\n");
	fgets(str,50,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	int len=strlen(str),wlen;;
	char *word;
	for(int i=0;str[i];i++)
	{
		if(str[i]>='0' && str[i]<='9')
		{
			word=convert(str[i]);
			wlen=strlen(word);
			memmove(str+i+wlen,str+i+1,strlen(str+i+1)+1);
			strncpy(str+i,word,wlen);
			i=i+wlen-1;
		}
	}
	printf("str:%s\n",str);
}

