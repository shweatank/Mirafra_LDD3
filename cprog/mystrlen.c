#include<stdio.h>

int mystrlen(const char *c)
{
	int count=0;
	while(*c)
	{
		count++;
		c++;
	}
	return count;
}
int main()
{
	char str[20];
	gets(str);
	int i=mystrlen(str);
	printf("len= %d\n",i);
	return 0;
}
