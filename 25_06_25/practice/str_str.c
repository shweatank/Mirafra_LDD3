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
char *str_str(char *haystack,char *needle)
{
	if(*needle=='\0')
	{
		return (char *)haystack;
	}
	for(;*haystack;haystack++)
	{
		const char *h=haystack;
		const char *n=needle;
		while(*h==*n&&*n!='\0')
		{
			h++;
			n++;
		}
		if(*n=='\0')
		{
			return (char *)haystack;
		}
	}
	return NULL;
}
int main()
{
	char str1[32],str2[32];
	int n;
	printf("Enter a string ");
	fgets(str1,sizeof(str1),stdin);
	remove_newline(str1);
	printf("Enter a string ");
	fgets(str2,sizeof(str2),stdin);
	remove_newline(str2);
	char *found=str_str(str1,str2);
	if(found!=NULL)
	{
		printf("Sub string found at %s\n",found);
	}
	else
	{
		printf("Sub string not found\n");
	}
	return 0;
}
