#include<stdio.h>
#include<string.h>

int main()
{
	FILE *fp;
	char *buf="Priyalatha";
	fp=fopen("text","w");
	if(!fp)
	{
		perror("fopen");
		return 0;
	}
	fwrite(buf,1,strlen(buf),fp);
	fclose(fp);

	fp=fopen("text","r");
	char str[30]={};
	fread(str,1,sizeof(str)-1,fp);
	printf("%s\n",str);
	fclose(fp);
}
