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
char *str_cpy(char *d,char *s)
{
	char *o_dst=d;
	while(*s)
	{
		*d=*s;
		s++;
		d++;
	}
	*d='\0';
	return o_dst;
}

int main()
{
	char str1[32],str2[32];
	printf("Enter a string ");
	fgets(str1,sizeof(str1),stdin);
	remove_newline(str1);
	str_cpy(str2,str1);
	printf("%s\n",str2);
	return 0;
}
