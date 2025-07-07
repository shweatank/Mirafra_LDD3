#include<stdio.h>
#include<string.h>
void rev(char *str)
{
	int i=0,j=strlen(str)-1;
	char ch;
	for(;i<j;i++,j--)
	{
		ch=str[i];
		str[i]=str[j];
		str[j]=ch;
	}
}
void myitob(int num,char *str)
{
	int i=0;
	while(num)
	{
		str[i++]=(num%2)+48;
		num/=2;
	}
	str[i]='\0';
	rev(str);
}
int main()
{
	int num;
	printf("enter num:\n");
	scanf("%d",&num);
	char str[100];
	myitob(num,str);
	printf("binary:%s\n",str);
}
