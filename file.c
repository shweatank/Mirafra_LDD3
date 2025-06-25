#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main()
{
	FILE *fp;
	fp=fopen("ex.txt","w");
	char wstr[100];
	char rstr[100];
	printf("Enter the data:\n");
	fgets(wstr,100,stdin);
	fwrite(wstr,sizeof(char),strlen(wstr)+1,fp);
	fclose(fp);
	fp=fopen("ex.txt","r");
	fread(rstr,sizeof(char),sizeof(rstr),fp);
	printf("%s\n",rstr);
}


