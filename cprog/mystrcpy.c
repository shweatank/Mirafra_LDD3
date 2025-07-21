#include<stdio.h>
#include<string.h>

char *mystrcpy(char *dst , char *src)
{
	while(*src)
	{
		*dst=*src;
		dst++;
		src++;
	}
	*dst='\0';
	return dst;

}
char *mystrcpy1(char *dst,char *src)
{
	dst=memmove(dst,src,strlen(src));
	dst[strlen(src)]='\0';
	return dst;
}
int main()
{
	char str[10],dst[10];
	gets(str);
	mystrcpy(dst,str);
	puts(dst);
}
