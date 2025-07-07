#include<stdio.h>
#include<string.h>
int my_btoi(char *str)
{
	int res=0,i=0,j=strlen(str)-1,val=1;
	char *p=str;
	for(;j>=i;j--)
	{
		if(str[j]=='1')
			res+=val;
		val*=2;
	}
	return res;
}
int main()
{
	char str[40];
	printf("enter a binary num:\n");
	fgets(str,40,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	int res=my_btoi(str);
	printf("decimal number:%d\n",res);
}
