//24 June 2025
//file handling using C library functions
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
void main()
{
	FILE *fp=fopen("text.txt","w+");
	perror("fopen");
	char str[20]="Mirafra Technologies";
	char out[6];
	fwrite(str,1,strlen(str),fp);
	rewind(fp);
	fread(out,1,5,fp);
	printf("%s\n",out);
	fclose(fp);
}
