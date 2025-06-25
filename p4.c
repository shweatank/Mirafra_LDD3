#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
	char buffer[100];
	char data[50]="Hello world";
FILE *fp=fopen("data","w+");
if(fp==0)
{
	perror("File is not opened\n");
	return 0;

}
fwrite(data,sizeof(char),strlen(data),fp);
fclose(fp);

fp=fopen("data","r");
if(fp==0)
{
	perror("file is not opened\n");
	return 0;
}
fread(buffer,sizeof(char),sizeof(buffer)-1,fp);
buffer[sizeof(buffer)-1]='\0';
fclose(fp);
printf("Data:%s\n",buffer);
}

