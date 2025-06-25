#include<stdio.h>
#include<unistd.h>
void main()
{
	FILE *fp;
	fp=fopen("file.txt","r");
	char ch;
	sleep(100);
	while((ch=getc(fp))!=EOF)
	{
		printf("%c",ch);
	}
	fclose(fp);
}

