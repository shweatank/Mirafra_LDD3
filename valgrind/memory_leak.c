#include<stdio.h>
#include<stdlib.h>
int main()
{
	printf("%d\n",getpid());
	int *ptr=malloc(10*sizeof(int));
	ptr[0]=5;
	free(ptr);
	return 0;
}
//memory is not freed
