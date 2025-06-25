#include<stdio.h>

int main()
{
	FILE *fp;
	char text[100];

	fp = fopen("temp.txt","w");
	if(fp==NULL)
	{
		printf("ERROR 1\n");
		return 1;
	}
	fprintf(fp,"Hello Mic Check\n");
	fprintf(fp,"Done\n");
	fclose(fp);

	fp = fopen("temp.txt","r");
	if(fp==NULL)
	{
		printf("ERROR2 \n");
		return 1;
	}
	while(fgets(text,sizeof(text),fp)!=NULL)
	{
		printf("%s",text);
	}
	return 0;
}
