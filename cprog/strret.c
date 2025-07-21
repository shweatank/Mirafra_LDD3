#include<stdio.h>
#include<string.h>
char *returnfunc(void)
{
	static char str[10];
	gets(str);
	return str;
}

int main()
{
	char *str;
	str=returnfunc();
	puts(str);
}
