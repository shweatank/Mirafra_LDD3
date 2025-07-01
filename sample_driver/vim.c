#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
void main()
{
	FILE *fp=fopen("/dev/simple_char_devv","a+");
	char str[10];
	printf("Enter the string :");
	scanf("%s",str);
	fwrite(str,1,strlen(str),fp);
	fclose(fp);
}
