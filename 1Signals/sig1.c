#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void function()
{
	printf("THANKS FOR PRESSING\n");
	exit(1);
}
int main()
{
	printf("CTRL +C gives the SIGINT\n");
	signal(SIGINT,function);
	while(1)
	{
		printf("Generate SIGINT\n");
	}
}
