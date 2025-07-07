#include<stdio.h>
#include<string.h>
int decimal_range(char *str,int i,int j)
{
	int k,res=0;;
	for(k=i;k<=j;k++)
	{
		if(str[k]=='1')
			res=res*2+1;
		else if(str[k]=='0')
			res=res*2;
	}
	return res;
}
int main()
{
	int i,j;
	char str[40];
	printf("enter a binary num:\n");
	fgets(str,40,stdin);
	if(str[strlen(str)-1]=='\n')
		str[strlen(str)-1]='\0';
	printf("enter i and j:\n");
	scanf("%d%d",&i,&j);
	int res=decimal_range(str,i,j);
	printf("res:%d\n",res);
}
