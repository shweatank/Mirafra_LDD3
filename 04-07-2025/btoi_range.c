#include<stdio.h>
#include<string.h>
int my_btoi(char *str,int start,int end)
{
	int len=strlen(str);
	int res=0,val=1,i;
	for(i=len-1-start;i>=len-1-end;i--)
	{
		if(str[i]=='1')
			res=res*2+1;
		else if(str[i]=='0')
			res=res*2;
		val*=2;
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
	int res=my_btoi(str,i,j);
	printf("decimal number:%d\n",res);
}
