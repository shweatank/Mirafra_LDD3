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
char *strn_cat(char *s1,char *s2,int n)
{
	char *s=s1;
	while(*s1)
	{
		s1++;
	}
	while(n--&&*s2)
	{
		*s1=*s2;
		s1++;
		s2++;
	}
	*s1='\0';
	return s;
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
	printf("%s\n",strn_cat(str1,str2,3));
	//printf("%s\n",str1);
	return 0;
}
