#include<stdio.h>
void main()
{
	FILE *fp;
	fp=fopen("file.txt","w");
	char buf[32];
	printf("Enter data ");
	fgets(buf,sizeof(buf),stdin);
	fwrite(buf,sizeof(char),sizeof(buf),fp);
	fclose(fp);
}
