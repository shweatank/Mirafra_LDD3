#include<stdio.h>
int main()
{
	FILE *fp=fopen("temp","w");

	char a[20]="hello";
	fwrite(a,sizeof(char),sizeof(a),fp);
	
	fclose(fp);

	FILE *fr=fopen("temp","r");

	char b[20];

	fread(b,sizeof(char),sizeof(b),fr);

	printf("%s",b);

	fclose(fr);
}
