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
int str_cmp(char *s1,char *s2)
{
	while(*s1&&*s2)
	{
		if(*s1!=*s2)
		{
			return *s1-*s2;
		}
		s1++;
		s2++;
	}
	return *s1-*s2;
}
int main()
{
	char str1[32],str2[32];
	printf("Enter a string ");
	fgets(str1,sizeof(str1),stdin);
	remove_newline(str1);
	printf("Enter a string ");
	fgets(str2,sizeof(str2),stdin);
	remove_newline(str2);
	printf("%d\n",str_cmp(str1,str2));
	return 0;
}
