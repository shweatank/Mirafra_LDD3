#include<stdio.h>
#include<string.h>
/*int main()
{
	FILE *fp;
	fp=fopen("data.txt","w");
	char str[100];
	fgets(str,100,stdin);
	if(fwrite(str,strlen(str),1,fp)==0)
		perror("fwrite");
	fclose(fp);
	fp=fopen("data.txt","r");
	char str1[100];
	if(fread(str1,100,1,fp)==0)
		perror("fread");
	puts(str1);
	fclose(fp);
}*/

int main()
{
	int fp;
	fp=open("data.txt",O_WRONLY|O_CREAT|O_EXCL,0664);

}
