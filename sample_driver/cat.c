#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
void main()
{
	char sh[10];
	FILE *fp=fopen("/dev/simple_char_devv","r");
	fread(sh,1,5,fp);
	printf("Result = %s\n",sh);
	fclose(fp);

}
