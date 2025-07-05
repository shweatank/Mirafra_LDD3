#include<stdio.h>
#include<stdlib.h>
int main()
{
	char buf[]="ada78876sds";
	memcpy(buf+1,buf,9);//overlapping the memcpy
	printf("%s\n",buf);
}

