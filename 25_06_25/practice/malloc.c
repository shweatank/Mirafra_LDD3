#include<stdio.h>
#include<stdlib.h>
int main()
{
	int *ptr=(int *)malloc(sizeof(int));
	*ptr=20;
	printf("%d\n",*ptr);
	free(ptr);
	return 0;
}
