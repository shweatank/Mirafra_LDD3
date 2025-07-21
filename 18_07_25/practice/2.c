#include<stdio.h>
extern void (*fptr)();
void main()
{
	/*extern int *ptr;
	*ptr+=5;
	printf("%d\n",*ptr);*/
	fptr();
}
