//static int a=10;
//int *ptr=&a;
#include<stdio.h>
static void fun()
{
	printf("Hello world\n");
}
void (*fptr)()=fun;
