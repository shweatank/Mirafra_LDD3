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
char *strn_cpy(char *d,char *s,int n)
{
	char *o_dst=d;
	while(n--&&*s)
	{
		*d=*s;
		d++;
		s++;
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
	printf("%s\n",strn_cpy(str2,str1,5));
	return 0;
}
