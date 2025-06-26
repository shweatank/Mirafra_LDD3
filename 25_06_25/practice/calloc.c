#include<stdio.h>
#include<stdlib.h>
int main()
{
	int *ptr=(int *)calloc(2,sizeof(int));
	ptr++;
	printf("%d\n",*ptr);
	return 0;
}
