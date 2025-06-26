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
int strn_cmp(char *s1,char *s2,int n)
{
	while(n)
	{
		if(*s1!=*s2)
		{
			return *s1-*s2;
		}
		s1++;
		s2++;
		n--;
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
	int index=strn_cmp(str1,str2,3);
	if(index==0)
	{
		printf("Both strings are equal\n");
	}
	else
	{
		printf("Difference is %d\n",index);
	}
	return 0;
}
