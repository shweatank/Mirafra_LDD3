#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
	char buf[20],temp[20];
	FILE *ptr=fopen("data.txt","w");
	gets(buf);
	if(ptr==NULL)
	{
	   perror("message");
	   return 0;
	}
	fwrite(buf,strlen(buf),1,ptr);
	fclose(ptr);
	ptr=fopen("data.txt","r");
	if(ptr==NULL)
	{
		perror("not found");
		exit(0);
	}
	fread(temp,strlen(buf),1,ptr);
	fclose(ptr);
	puts(temp);
}
	

