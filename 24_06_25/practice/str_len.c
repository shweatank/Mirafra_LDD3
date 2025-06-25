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
int str_len(char *s)
{
	int count=0;
	while(*s)
	{
		count++;
		s++;
	}
	return count;
}
int main()
{
	char str[32];
	printf("Enter a string ");
	fgets(str,sizeof(str),stdin);
	remove_newline(str);
	printf("%d\n",str_len(str));
	return 0;
}
