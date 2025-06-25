#include<stdio.h>
void remove_newline(char s[])
{
	int i=0;
	while(s[i])
	{
		if(s[i]=='\n'||s[i]=='\0')
		{
			s[i]='\0';
			break;
		}
		i++;
	}
}
char *str_cat(char *s1,char *s2)
{
	char *str=s1;
	int i=0,j=0;
	while(s1[i])
	{
		i++;
	}
	while(s2[j])
	{
		s1[i]=s2[j];
		i++;
		j++;
	}
	s1[i]='\0';
	return str;
}
int main()
{
	char str1[32],str2[32];
	printf("Enter first string ");
	fgets(str1,sizeof(str1),stdin);
	remove_newline(str1);
	printf("Enter first string ");
	fgets(str2,sizeof(str2),stdin);
	remove_newline(str2);
	printf("%s\n",str_cat(str1,str2));
	return 0;
}
