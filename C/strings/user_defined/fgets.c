#include<stdio.h>
char *myfgets(char *str,int n)
{
	unsigned char ch,i;
	do{
		ch=getchar();
		if(ch==EOF)
			break;
		str[i]=ch;
	}while(i<n-1 && str[i++]!='\n');
	str[i]='\0';
	if(i==0 || ch==EOF)
		return NULL;
}
int main()
{
	char str[10];
	printf("enter str:\n");
	myfgets(str,10);
	printf("%s",str);
}
