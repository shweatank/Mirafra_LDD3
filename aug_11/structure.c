#include<stdio.h>
#include<string.h>
struct data
{
	char str[10];
	char str2[10];
	char str3[10];
};

int main()
{
	struct data d;
	char *buf="hihellobye4";
        strncpy(d.str,buf,2);
	d.str[2]='\0';
	strncpy(d.str2,buf+2,5);
	d.str2[6]='\0';
	strncpy(d.str3,buf+7,3);
	d.str3[3]='\0';
	printf("str:%s\nstr2:%s\nstr3:%s\n",d.str,d.str2,d.str3);
} 
