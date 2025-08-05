#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void memory_leak()
{
	char *ptr1=malloc(50);
	strcpy(ptr1,"This is memory leak example");

	char *ptr2=malloc(100);
	strcpy(ptr2,"Another leaked block");

	free(ptr1);
	free(ptr2);

}

int main()
{
	printf("Memory leak example\n");
	memory_leak();
	printf("Done\n");
	return 0;
}
