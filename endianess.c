#include<stdio.h>
int main()
{
	int num=1;
	char *ptr=(char *)&num;

	if(*ptr==1)
	{
		printf("System is following little Endian\n");
	}
	else
	{
		printf("System is following Big Endian\n");
	}
	return 0;
}

