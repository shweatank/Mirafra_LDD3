#include<stdio.h>
#include<stdlib.h>
int main()
{
	int *arr=calloc(1,3*sizeof(int));
	arr[2]=21;//out of bounds 
        free(arr);
}
//invalid access outof bounds accessing memory

