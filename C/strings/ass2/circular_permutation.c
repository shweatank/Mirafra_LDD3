#include<stdio.h>
#include<string.h>
int circular_permutation(char *str1,char *str2)
{
	int len1=strlen(str1);
	int len2=strlen(str2),len3;
	if(len1!=len2)
		return 0;
	len3=(2*len1)+1;
	char str3[len3];
	strcpy(str3,str1);
	strcat(str3,str1);
	if(strstr(str3,str2))
		return 1;
	return 0;
}
int main()
{
	char str1[50],str2[50];
	printf("enter str1:\n");
	fgets(str1,50,stdin);
	if(str1[strlen(str1)-1]=='\n')
		str1[strlen(str1)-1]='\0';
	printf("enter str2:\n");
	fgets(str2,50,stdin);
	if(str2[strlen(str2)-1]=='\n')
		str2[strlen(str2)-1]='\0';
	if(circular_permutation(str1,str2))
		printf("yes str2 is circular permutation of str1\n");
	else
		printf("NO\n");
}
