#include<stdio.h>
typedef union 
{
	int i;
	char ch;
}un;
int main()
{
	un var;
	var.i=1;
	if(var.ch==1)
		printf("little endian\n");
	else
		printf("big endian\n");
}
