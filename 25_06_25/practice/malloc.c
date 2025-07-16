#include<stdio.h>
#include<stdlib.h>
int main()
{
	int *ptr=(int *)malloc(sizeof(int));;
	printf("%d\n",*ptr);
	free(ptr);
	return 0;
}
